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

function rand() {
    return Math.random();
}

// function starts to run once the file is ready
$(document).ready(function() {

    Plotly.newPlot('car_speed', [{
        x: [],
        y: [],
        name: 'Speed (m/s)',
        mode: 'lines',
    }], {
        title: { text: 'Speed' },
        showlegend: true
    });

    Plotly.newPlot('bms_module_min_max', [{
        x: [],
        y: [],
        name: 'Min Module Voltage (V)',
        mode: 'lines',
    }, {
        x: [],
        y: [],
        name: 'Max Module Voltage (V)',
        mode: 'lines',
    }, {
        x: [],
        y: [],
        name: 'Min Module Temp (DegC)',
        mode: 'lines',
        yaxis: 'y2',
    }, {
        x: [],
        y: [],
        name: 'Max Module Temp (DegC)',
        mode: 'lines',
        yaxis: 'y2',
    }], {
        title: { text: 'BMS Module Min/Max' },
        showlegend: true,
        yaxis2: { side: 'right', overlaying: 'y', }
    });

    Plotly.newPlot('bms_measurement', [{
        x: [],
        y: [],
        name: 'Battery Current (A)',
        mode: 'lines',
    }, {
        x: [],
        y: [],
        name: 'Battery Voltage (V)',
        mode: 'lines',
        yaxis: 'y2',
    }], {
        title: { text: 'BMS Measurement' },
        showlegend: true,
        yaxis2: { side: 'right', overlaying: 'y', }
    });

    Plotly.newPlot('bms_capacity', [{
        x: [],
        y: [],
        name: 'Battery Capcity (Wh)',
        mode: 'lines',
    }], {
        title: { text: 'BMS Capacity' },
        showlegend: true
    });

	streamPackets(function(data) {
        console.log(data);

        var time = new Date();

        const car_speed = data.dashboard?.car_speed?.[0];
        const bms_module_min_max = data.bms?.bms_module_min_max?.[0];
        const bms_measurement = data.bms?.bms_measurment?.[0];
        const bms_capacity = data.bms?.bms_capacity?.[0];
        // TODO: array power

        if (car_speed !== undefined) {
            // console.log(car_speed.speed);
            var update = {
                x:  [[time]],
                y: [[car_speed.speed]]
            };
            Plotly.extendTraces('bms_measurement', update, [0]);
        }

        if (bms_module_min_max !== undefined) {
            // console.log(bms_module_min_max.module_max_temp);
            var update = {
                x:  [[time], [time], [time], [time]],
                y: [[bms_module_min_max.module_min_voltage], [bms_module_min_max.module_max_voltage], [bms_module_min_max.module_min_temp], [bms_module_min_max.module_max_temp]]
            };
            Plotly.extendTraces('bms_module_min_max', update, [0, 1, 2, 3]);
        }

        if (bms_measurement !== undefined) {
            // console.log(bms_measurement.current);
            var update = {
                x:  [[time], [time]],
                y: [[bms_measurement.current], [bms_measurement.battery_voltage]]
            };
            Plotly.extendTraces('bms_measurement', update, [0, 1]);
        }

        if (bms_capacity !== undefined) {
            var update = {
                x:  [[time]],
                y: [[bms_capacity.Wh]]
            }
            Plotly.extendTraces('bms_capacity', update, [0])
        }
        
	});

});