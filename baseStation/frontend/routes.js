var express = require('express');
var router = express.Router();
var request = require('request');
const HOST = "HTTP://localhost:3030";
router.route('/devices')
    // get all the devices (accessed at GET http://localhost:3030/station/devices)
    .get(function (req, res) {
        request({
            url: HOST + '/api/device',
            json: true
        }, function (error, response, body) {

            if (!error && response.statusCode === 200) {
                res.render('list', {devices: body});
            }
        })
    });

// on routes that end in /station/devices/:id
// ----------------------------------------------------
router.route('/devices/:id')
    .get(function (req, res) {
        request({
            url: HOST + '/api/device/' + req.params.id,
            json: true
        }, function (error, response, body) {

            if (!error && response.statusCode === 200) {
                res.render('detail', {device: body, url: HOST + '/api/device/' + req.params.id});
            }
        })
    });


module.exports = router;

