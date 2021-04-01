# wbsrvr

A node backend that hosts an HTTP server, a websocket server and a UDP socket which all use the same IP address however UDP binds on a port 8082 while express and the websocket operates on port 3000.

The aim of this app is to display real-time data coming from a MCU over a UDP socket and to provide a UI to configure the ADC and IIR filter section.

The app.js file configures the express environment for node.

The views directory contains the public HTML files that are displayed in the browser.

The routes directory is responsible for separating the middleware structure of different enpoints allowing transparent programming environment.


