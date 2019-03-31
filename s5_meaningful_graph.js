var express = require('express')
var app = express()
fs = require('fs');
mysql = require('mysql');
PrivateData = require('./PrivateData.js');
var connection = mysql.createConnection({
    host: PrivateData.host_data,
    user: PrivateData.user_data,
    password: PrivateData.pwd_data,
    database: 'Temperature'
})
connection.connect();


app.get('/graph', function (req, res) {
    console.log('got app.get(graph)');
    var html = fs.readFile('./graph.html', function (err, html) {
    html = " "+ html
    console.log('read file');

    var qstr = 'select * from sensors ';
    connection.query(qstr, function(err, rows, cols) {
      if (err) throw err;

      var data = "";
      var comma = ""
      var A;
      var B;
      for (var i=0; i< rows.length; i++) {
         r = rows[i];
	 A = r.CurrentDate[4];
	 B = r.CurrentDate[5];
	 B-=1;
	 if(r.CurrentDate[5] < 0) {
		 A-=1;
		 B += 10;
	 }
//         data += comma + "[new Date(2019,03-2,"+ r.id +",00,38),"+ r.value +"]";
	 data += comma + "[new Date(" + r.CurrentDate[0] + r.CurrentDate[1] + r.CurrentDate[2] + r.CurrentDate[3]
	      + "," + A + B + "," + r.CurrentDate[6] + r.CurrentDate[7]
	      + "," + r.CurrentTime[0] + r.CurrentTime[1] + "," + r.CurrentTime[2] + r.CurrentTime[3]
	      + "," + r.CurrentTime[4] + r.CurrentTime[5] + ")," + r.Temperature + "]";
         comma = ",";
      }
      var header = "data.addColumn('date', 'Date/Time');"
      header += "data.addColumn('number', 'Temperature');"
      html = html.replace("<%HEADER%>", header);
      html = html.replace("<%DATA%>", data);

      res.writeHeader(200, {"Content-Type": "text/html"});
      res.write(html);
      res.end();
    });
  });
})

var server = app.listen(3001, function () {
//  var host = server.address().address
//  var port = server.address().port
  console.log('listening')
});
