# wbsrvr

A node backend hosts an HTTP server, a websocket server and a UDP socket which all use the same IP address however UDP binds on a port 8082 while express and the websocket operates on port 3000.

The app.js file configures the express environment for node.

The views directory contains the public HTML files that are displayed in the browser.

The routes directory is responsible for separating the middleware structure of different enpoints allowing transparent programming environment.


