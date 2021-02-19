const { exec } = require("child_process");

var user = "+18572850070";

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
    console.log("length:",stdout.length);
    var len = stdout.length;
    if (len>0) {
        const obj = JSON.parse(stdout);
        var env = obj.envelope;
        var source = env.source;
        var msg = env.syncMessage.sentMessage.message;
        console.log(source+": "+msg);
        
    }
});