
const yargs = require('yargs/yargs')
const { hideBin } = require('yargs/helpers')
const argv = yargs(hideBin(process.argv)).argv

const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')
const { exec } = require("child_process");

function getSignalMessagesAndPushToSerialPort() {

    const port = new SerialPort(argv.port, function (err) {
        if (err) {
          return console.log('Error: ', err.message)
        }
    
        var user = argv.user;
    
    var receive_command = "signal-cli -u "+user+" -o json receive";
    
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
    
      });
}

function getFormattedDate(unix_timestamp) {

    var date = new Date(unix_timestamp);
    // Hours part from the timestamp
    console.log("timestamp=",unix_timestamp);
    var weekdays = new Array(7);
    weekdays[0] = "Sun";
    weekdays[1] = "Mon";
    weekdays[2] = "Tues";
    weekdays[3] = "Wed";
    weekdays[4] = "Thu";
    weekdays[5] = "Fri";
    weekdays[6] = "Sat";
    
    var months = new Array(12);
    months = ["Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"];
    
    var day = weekdays[date.getDay()];
    
    var num_day = date.getDate();
    
    var month = months[date.getMonth()];
    var month_number = date.getMonth()+1;
    var hours = date.getHours();
    // Minutes part from the timestamp
    var minutes = "0" + date.getMinutes();
    // Seconds part from the timestamp
    var seconds = "0" + date.getSeconds();
    
    // Will display time in 10:30:23 format
    //var formattedTime = day + " " + month + " " + num_day + " " + hours + ':' + minutes.substr(-2) + ':' + seconds.substr(-2);
    var formattedTime = month_number+"-"+num_day + " " + hours + ':' + minutes.substr(-2) + ':' + seconds.substr(-2);

    console.log(formattedTime);

    return formattedTime;

}


function processSignal(item, index) {
    if (item.length > 2) {
    const obj = JSON.parse(item);
    console.log(obj);
    var env = obj.envelope;
    var source = env.source;
    if (env.hasOwnProperty('dataMessage')) {
        var msg = env.dataMessage.message;
        var timestamp = getFormattedDate(env.dataMessage.timestamp);
        var outString = '{"source":"'+source+'","timestamp":"'+timestamp+'","msg":"'+msg+'"}'
        //var outString = "<"+msg+",12,24.7>";
        console.log(outString);
        //port.write(outString+"\n");
        port.write(outString);
    }
    if (env.hasOwnProperty('syncMessage')) {
    var sync = env.syncMessage;
    if (sync.hasOwnProperty('sentMessage')) {
    var msg = sync.sentMessage.message;
    var timestamp = getFormattedDate(sync.sentMessage.timestamp);
    
    //var outString = source+": "+msg;
    //var outString = "asdlkjasdsdflkjsdflksjdflkjasdfasdfasfsad";
    var outString = '{"source":"'+source+'","timestamp":"'+timestamp+'","msg":"'+msg+'"}'
    //var outString = "<"+msg+",12,24.7>";
    console.log(outString);
    //port.write(outString+"\n");
    port.write(outString);
}
    }
}
}

if(argv.user && argv.port) {
    console.log("running in loop ...");
    setInterval(getSignalMessagesAndPushToSerialPort, 10000); // run every 5 seconds
}
else {
    console.log("need to specify --user and --port");
}