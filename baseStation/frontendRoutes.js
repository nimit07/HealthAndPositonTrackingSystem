// ROUTES FOR OUR API
// =============================================================================

// create our router
var express = require('express');
var router = express.Router();


// test route to make sure everything is working (accessed at GET http://localhost:3030/station)
router.get('/', function (req, res) {
    res.json({message: 'hooray! welcome to our frontend!'});
});

// on routes that end in /station/devices
// ----------------------------------------------------
router.use(require('./frontend/routes'));
// REGISTER OUR ROUTES -------------------------------
module.exports = router;
