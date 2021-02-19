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
    console.log("length:",stdout.length);
    var len = stdout.length;
    if (len>0) {
    const obj = JSON.parse(stdout);
    console.log(obj);
    var env = obj.envelope;
    var source = env.source;
    var msg = env.syncMessage.sentMessage.message;
    var outString = source+": "+msg;
    //var outString = "<"+msg+",12,24.7>";
    console.log(outString);
    //port.write(outString+"\n");
    port.write("HELLO "+outString+"\n");
    port.close(function (err) {
        console.log('port closed', err);
    });

    }
});

/* const parser = port.pipe(new Readline({ delimiter: '\r\n' }))
parser.on('data', function (data) {
console.log('Data:', data);
if (data=="4") {
    console.log("bang!");
}
});
 */



