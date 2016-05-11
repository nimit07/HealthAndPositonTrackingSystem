// BASE SETUP
// =============================================================================

// call the packages we need
var express = require('express');
var bodyParser = require('body-parser');
var app = express();
var morgan = require('morgan');
var path = require('path');
// configure app
app.use(morgan('dev')); // log requests to the console

// configure body parser
app.use(bodyParser.urlencoded({extended: true}));
app.use(bodyParser.json());
app.use(express.static(__dirname + '/frontend/public'));
// configuring static directory and template engine
app.set('views', path.join(__dirname, 'frontend/views'));
app.set('view engine', 'ejs');

var port = process.env.PORT || 3030; // set our port

var mongoose = require('mongoose');
mongoose.connect('mongodb://localhost:27017/health_monitor'); // connect to our database


// add our router
app.use('/api', require('./backendRoutes'));
app.use('/station', require('./frontendRoutes'));


// START THE SERVER
// =============================================================================
app.listen(port);
console.log('Magic happens on port ' + port);

