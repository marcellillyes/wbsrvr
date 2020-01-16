const { workerData, parentPort } = require('worker_threads')
const fs = require('fs');
const EventEmitter = require('events');

var w= fs.createWriteStream('C:\\Users\\illye\\IdeaProjects\\webserver\\Data' + '/'+ Date.now().toString() +'.txt');


class Queue extends EventEmitter {
    constructor(basePath, baseIndex, concurrent = 5) {
        super();
        this.q = [];
        this.paused = false;
        this.inFlightCntr = 0;
        this.fileCntr = baseIndex;
        this.maxConcurrent = concurrent;
    }

    // add item to the queue and write (if not already writing)
    add(data) {
        this.q.push(data);
        this.write();
    }

    // write next block from the queue (if not already writing)
    write() {
        while (!this.paused && this.q.length && this.inFlightCntr < this.maxConcurrent) {
            this.inFlightCntr++;
            let buf = this.q.shift();
            try {
                fs.writeFile(super.basePath + (Date.now()).toString(), buf, err => {
                    this.inFlightCntr--;
                    if (err) {
                        this.err(err);
                    } else {
                        // write more data
                        this.write();
                    }
                });
            } catch(e) {
                this.err(e);
            }
        }
    }

    err(e) {
        this.pause();
        this.emit('error', e)
    }

    pause() {
        this.paused = true;
    }

    resume() {
        this.paused = false;
        this.write();
    }
}
//let q = new Queue('C:\\Users\\illye\\IdeaProjects\\webserver\\Data',0,5);
//q.add(workerData);

/*fs.appendFile('C:\\Users\\illye\\IdeaProjects\\webserver\\Data' + '/work.txt', workerData,function (err) {
    if (err) throw err;
    //console.log(workerData);
});*/

w.write(workerData);

parentPort.postMessage("done");