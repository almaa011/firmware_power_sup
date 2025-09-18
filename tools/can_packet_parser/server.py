
import os
import sys

import http.server
import urllib.parse
import json
import time
from queue import Queue

from interface_digi_xbee import interface_digi_xbee
from can_packet_codec import XBeeCANPacketCodec
from scrappy import yamls_to_packets


class Logger:
    """
    Class to handle logging packets to a file.
    """

    def __init__(self, filename: str):
        self.log_file = open(filename, 'a')

    def send(self, packet):
        out = '%.3f ' % time.time()  # write time to 3 decimal places and leave a space
        out += '%03X' % packet.id  # write the id in hex using 3 digits
        for data in packet.data:
            out += '%02X' % data  # write the data in 
        out += '\n'
        # single write operation to reduce the chance that data is partially written in
        # the event the user terminates the program
        self.log_file.write(out)

    def close(self):
        self.log_file.close()


class QueuePool:
    """
    QueuePool holds a list of queues CAN packets will be sent to when received. This allows us to use a multi-threading HTTP server.
    """
    def __init__(self):
        self.pool = []

    def register(self):
        """
        Add a new queue to the pool and return that Queue.
        """
        queue = Queue()
        self.pool.append(queue)
        return queue

    def unregister(self, queue):
        self.pool.remove(queue)

    def send(self, packet):
        for queue in self.pool:
            queue.put(packet)


def build_handler(queue_pool):

    class Handler(http.server.SimpleHTTPRequestHandler):
        def do_GET(self):
            url = urllib.parse.urlparse(self.path)

            if url.path == '/stream':

                self.send_response(200)
                self.send_header('Content-type', 'text/event-stream')
                self.end_headers()

                queue = queue_pool.register()

                try:
                    while True:
                        packet = queue.get()  # this will block until a packet is available
                        self.wfile.write(b"retry: 250\n")
                        self.wfile.write(b"event: message\n")
                        self.wfile.write(b"data: ")
                        self.wfile.write(f'{json.dumps(packet)}'.encode('utf-8'))
                        self.wfile.write(b"\n\n")
                except IOError:
                    queue_pool.unregister(queue)
            
            else:

                mimetype = "text/html"

                try:

                    if url.path == "/":

                        with open(os.path.join('www', 'index.html'), 'rb') as f:
                            content = f.read()

                    else:

                        ext = url.path.split(".")[-1]

                        if ext == "png":
                            mimetype = "image/png"
                        elif ext == "gif":
                            mimetype = "image/gif"
                        elif ext == "js":
                            mimetype = "application/javascript"
                        elif ext == "css":
                            mimetype = "text/css"

                        with open('www' + url.path, 'rb') as f:
                            content = f.read()

                        print(os.curdir + os.sep + "www" + url.path)

                except IOError:

                    self.send_response(404)
                    return

                self.send_response(200)
                self.send_header("Content-type", mimetype)
                self.end_headers()

                self.wfile.write(content)

    return Handler


def build_xbee_handler(packets, queue_pool, logger):
    def xbee_handler(packet):
        can_packet_t = packets.get(packet.id, None)

        logger.send(packet)

        while len(packet.data) < 8:
            packet.data.append(0)
        packet.data = bytes(packet.data)

        if can_packet_t is not None:
            parsed_packet = can_packet_t.parse_packet_to_json(
                packet.id, packet.data
            )
            queue_pool.send(parsed_packet)
        else:
            string = "WARNING UNKNOWN PACKET: {0}  {1:8X}   [{2}]  {3}".format(
                'xbee',
                packet.id,
                len(packet.data),
                " ".join(["{0:02X}".format(b) for b in packet.data]),
            )
            print(string)

    return xbee_handler


def main():

    # We will use port 8000, this can change if needed
    PORT = 8000

    queue_pool = QueuePool()

    # Create the server to run at localhost:PORT using the Handler from above
    server = http.server.ThreadingHTTPServer(('localhost', PORT), build_handler(queue_pool))

    # Dictionary of packets found in the provided directory
    packets = yamls_to_packets(sys.argv[2])

    # Create a Logger object to log to the provided file
    logger = Logger(sys.argv[3])

    com_port = sys.argv[1]

    interface = interface_digi_xbee(com_port, build_xbee_handler(packets, queue_pool, logger), codec=XBeeCANPacketCodec())

    # start the thread
    interface.start()

    print(f'Telemetry server available at http://localhost:{PORT}/')

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        interface.stop()
        logger.close()


if __name__ == "__main__":
    main()
