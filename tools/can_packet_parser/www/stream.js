
var json = {};

function streamPackets(callback) {
    if (!!window.EventSource) {
        // Create an EventSource object that will listen to /stream
		var jsonStream = new EventSource('/stream');
        // This event listener will listen for 'message' events
		jsonStream.addEventListener('message', function(e) {
			var packet = $.parseJSON(e.data);
		    callback(packet);
		});
    }
}

// function starts to run once the file is ready
$(document).ready(function() {

	streamPackets(function(data) {
        console.log(data);

        for (const board in data) {
            if (!(board in json)) {
                json[board] = {};
            }
            for (const message in data[board]) {
                if (!(message in json[board])) {
                    json[board][message] = {};
                }
                for (const idx in data[board][message]) {
                    json[board][message][idx] = data[board][message][idx];
                }
            }
        }

        $('#json-renderer').jsonViewer(json);
        
	});

});