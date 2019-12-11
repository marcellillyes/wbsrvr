var express = require('express');
var router = express.Router();
var {PythonShell} = require('python-shell');
var WebSocket = require('ws');


var options = {
  mode: 'json',
  pythonPath: 'C:\\Users\\illye\\AppData\\Local\\Programs\\Python\\Python38-32\\python.exe',
  pythonOptions: ['-u'], // get print results in real-time
  scriptPath: 'C:\\Users\\illye\\IdeaProjects\\webserver',
  args: ['value1', 'value2', 'value3']
};

var pyshell = new PythonShell('processing.py', options);



pyshell.on('message', function (message) {
  // received a message sent from the Python script (a simple "print" statement)
  console.log(JSON.stringify(message));
});

/* GET users listing. */
router.get('/', function(req, res, next) {
  res.render('ADCSettings');
  console.log(2);
});

router.post("/", function (req, res, next) {
  console.log(req.body);
  pyshell.send(req.body);

  res.render('ADCSettings');

});

module.exports = router;
