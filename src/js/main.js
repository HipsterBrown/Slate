/* jshint latedef:false */
/* global Pebble */

var parseReminder = require('./parse-reminder')

// Set callback for the app ready event
Pebble.addEventListener("ready", function(e) {
  console.log("connect! " + e.ready);
  console.log(e.type);
});

// Set callback for appmessage events
Pebble.addEventListener("appmessage", function(e) {
  checkText(e.payload["0"]);
});

function checkText(text) {
  if (typeof text === 'string' && text.length > 0 && text.toLowerCase().indexOf('remind') > -1) {
    Pebble.sendAppMessage({
      "0": "Message Received!"
    });
    
    createReminderPin(text.toLowerCase());
  } else {
    Pebble.sendAppMessage({
      "0": 'Reminders should start with "Remind me".'
    });
  }
}

function createReminderPin(text) {
  var reminder = parseReminder(text);

  if (reminder.error) {
    Pebble.sendAppMessage({
      "0": reminder.error
    });
    return;
  }
  var time = reminder.datetime;
  var task = reminder.task;

  var pin = {
    "id": "pin-" + time,
    "time": time,
    "layout": {
      "type": "genericPin",
      "title": task,
      "tinyIcon": "system://images/ALARM_CLOCK"
    },
    "createNotification": {
      "layout": {
        "type": "genericNotification",
        "title": "New Reminder!",
        "tinyIcon": "system://images/NOTIFICATION_FLAG",
        "body": "Your reminder has been saved."
      }
    },
    "reminders": [
      {
        "time": time,
        "layout": {
          "type": "genericReminder",
          "title": task,
          "tinyIcon": "system://images/ALARM_CLOCK"
        }
      }
    ]
  }
  
  sendPin(pin);
}

function sendPin(pin) {
  var API_URL_ROOT = 'https://timeline-api.getpebble.com/';
  var url = API_URL_ROOT + 'v1/user/pins/' + pin.id;
  
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    console.log('timeline: response received: ' + this.responseText);
    Pebble.sendAppMessage({
      "0": "Reminder Set!"
    });
  };
  xhr.onerror = function (e) {
    console.log(JSON.stringify(e));
  };
  xhr.open('PUT', url);
  
  Pebble.getTimelineToken(function (token) {
    console.log('Token: ' + token);
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.setRequestHeader('X-User-Token', '' + token);
    
    xhr.send(JSON.stringify(pin));
    console.log('Request sent!');
  }, function (error) { console.log("Timeline Token error:\n" + error); });
}
