var express = require('express');
var router = express.Router();
var {PythonShell} = require('python-shell');
var WebSocket = require('ws');

//creating websocketclient that is connected to server
const ws = new WebSocket('ws://localhost:3000', )

//Python shell options
var options = {
  mode: 'json',
  pythonPath: 'C:\\Users\\illye\\AppData\\Local\\Programs\\Python\\Python38-32\\python.exe',
  pythonOptions: ['-u'], // get print results in real-time
  scriptPath: 'C:\\Users\\illye\\IdeaProjects\\webserver',
  args: ['value1', 'value2', 'value3']
};

var pyshell = new PythonShell('processing.py', options);

//Python shell event listener sends incoming json objects to server (IIR scaled up coefficients)
pyshell.on('message', function (message) {
  ws.send(JSON.stringify(message),{}, function () {
  });
});

/* GET users listing. */
router.get('/', function(req, res, next) {
  res.render('ADCSettings');
});

router.post("/", function (req, res, next) {
  console.log(req.body);
  pyshell.send(req.body);
  res.render('ADCSettings');
});

module.exports = router;
