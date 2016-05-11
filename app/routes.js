//required modules
var express = require('express');
var router = express.Router();
var Device = require('./model');
router.route('/device')
    //add new Device (accessed at POST http://localhost:3030/api/devices)
    .post(function (req, res) {

        var device = new Device();		// create a new instance of the Device model
        console.log(req.body);
        device.name = req.body.name;  // set the device name (comes from the request)
        if (req.body.pulse)
            device.pulse = req.body.pulse;
        if (req.body.gpsData)
            device.gpsData = req.body.gpsData;
        if (req.body.sosState)
            device.sosState = req.body.sosState;
        device.save(function (err) {
            if (err)
                res.send(err);

            res.json({message: 'Device added!'});
        });
    })

    // get all the devices (accessed at GET http://localhost:3030/api/devices)
    .get(function (req, res) {
        Device.find(function (err, devices) {
            if (err)
                res.send(err);

            res.json(devices);
        });
    });

// on routes that end in /devices/:id
// ----------------------------------------------------
router.route('/device/:id')

    // get the device with that id
    .get(function (req, res) {
        Device.findById(req.params.id, function (err, device) {
            if (err)
                res.send(err);
            console.log(req.query.upd);

            res.json(device);
        });
    })
    // update the device with this id
    .put(function (req, res) {
        Device.findById(req.params.id, function (err, device) {

            if (err)
                res.send(err);
            if (req.body.pulse)
                device.pulse = req.body.pulse;
            if (req.body.gpsData)
                device.gpsData = req.body.gpsData;
            if (req.body.sosState >= 1)
                device.sosState = 1;
            device.save(function (err) {
                if (err)
                    res.send(err);
                console.log(device);
                res.json({message: 'Device data updated!'});
            });

        });
    })

    // delete the device with this id
    .delete(function (req, res) {
        Device.remove({
            _id: req.params.id
        }, function (err, device) {
            if (err)
                res.send(err);

            res.json({message: 'Successfully deleted'});
        });
    });

module.exports = router 

