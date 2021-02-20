

function getFormattedDate(unix_timestamp) {

    var date = new Date(unix_timestamp);
    // Hours part from the timestamp
    
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

}

getFormattedDate(1613830776066);