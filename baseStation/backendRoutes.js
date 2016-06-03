// ROUTES FOR OUR API
// =============================================================================

// create our router
var express = require('express');
var router = express.Router();


// test route to make sure everything is working (accessed at GET http://localhost:8080/api)
router.get('/', function (req, res) {
    res.json({message: 'hooray! welcome to our api!'});
});

// on routes that end in /devices
// ----------------------------------------------------
router.use(require('./app/routes'));
// REGISTER OUR ROUTES -------------------------------
module.exports = router;
