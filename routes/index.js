var express = require('express');
var router = express.Router();

var address ="";

require('dns').lookup(require('os').hostname(), function (err, add, fam) {
  console.log('addr: '+add);
  address=add;
});


/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { address: address });
  console.log(2);
});

module.exports = router;
