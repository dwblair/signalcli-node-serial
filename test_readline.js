const SerialPort = require('serialport')
const port = new SerialPort('/dev/ttyUSB0', function (err) {
  if (err) {
    return console.log('Error: ', err.message)
  }
})

/*
// Switches the port into "flowing mode"
port.on('data', function (data) {
    console.log('Data:', data)
  })
*/
const Readline = require('@serialport/parser-readline')

const parser = port.pipe(new Readline({ delimiter: '\r\n' }))
parser.on('data', function (data) {
    console.log('Data:', data)
  })