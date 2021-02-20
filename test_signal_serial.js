const SerialPort = require('serialport')
const { exec } = require("child_process");

const port = new SerialPort('/dev/ttyUSB0', function (err) {
    if (err) {
      return console.log('Error: ', err.message)
    }
  });

var user = "+18572850070";

var receive_command = "signal-cli -u "+user+" -o json receive";

const Readline = require('@serialport/parser-readline')



exec(receive_command, (error, stdout, stderr) => {
    if (error) {
        console.log(`error: ${error.message}`);
        return;
    }
    if (stderr) {
        console.log(`stderr: ${stderr}`);
        return;
    }
    console.log(`stdout: ${stdout}`);
    var split_string = stdout.split('\n');
    console.log('split:',split_string);
    console.log("length:",stdout.length);
    var len = stdout.length;
    if (len>0) {
    split_string.forEach(processSignal);
    }
    port.close(function (err) {
        console.log('port closed', err);
    });
});


function processSignal(item, index) {
    if (item.length > 2) {
    const obj = JSON.parse(item);
    console.log(obj);
    var env = obj.envelope;
    var source = env.source;
    if (env.hasOwnProperty('syncMessage')) {
    var sync = env.syncMessage;
    if (sync.hasOwnProperty('sentMessage')) {
    var msg = sync.sentMessage.message;
    //var outString = source+": "+msg;
    //var outString = "asdlkjasdsdflkjsdflksjdflkjasdfasdfasfsad";
    var outString = '{"source":"'+source+'", "msg":"'+msg+'"}'
    //var outString = "<"+msg+",12,24.7>";
    console.log(outString);
    //port.write(outString+"\n");
    port.write(outString);
}
    }
}
}


/* const parser = port.pipe(new Readline({ delimiter: '\r\n' }))
parser.on('data', function (data) {
console.log('Data:', data);
if (data=="4") {
    console.log("bang!");
}
});
 */



