// Database Collection
var mongoose = require('mongoose');
var Schema = mongoose.Schema;

var DeviceSchema = new Schema({
    name: {
        type: String
    },
    pulse: {
        type: Number,
        'default': 0
    },
    gpsData: {
        type: {
            lat: {
                type: String,
                'default': 0
            },
            lon: {
                type: String,
                'default': 0
            },
            sat: {
                type: Number,
                'default': 0
            },
            prec: {
                type: Number,
                'default': 0
            }
        },
        required: true
    },
    sosState: {
        type: Number,
        'default': 0
    }
});

module.exports = mongoose.model('Device', DeviceSchema);
