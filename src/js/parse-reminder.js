module.exports = parseReminder

var date =  require('./date/parser')

function capitalizeFirstLetter(string) {
  return string.charAt(0).toUpperCase() + string.slice(1);
}

var rMeridiem = /(at)?\s?(\d{1,2})([:.](\d{1,2}))?([:.](\d{1,2}))?\s*([ap]m)/g;
var rHourMinute = /(\d{1,2})([:.](\d{1,2}))([:.](\d{1,2}))?/g;
var rAtHour = /(at|in)\s?(\d{1,2})/g;
var rDays = /(on)?\s?(sunday?|monday?|tuesday?|wednesday?|thursday?|friday?|saturday?)s?/g;
var rMonths = /(on the|on|the)?\s?((\d{1,2})(st|nd|rd|th))\sof\s(january|february|march|april|may|june|july|august|september|october|november|december)/;
var rDayMod = /(in the|this|at)?\s?(morning|noon|afternoon|night|evening|midnight)/;

var numberMap = {
  "one": "1",
  "two": "2",
  "three": "3",
  "four": "4",
  "five": "5",
  "six": "6",
  "seven": "7",
  "eight": "8",
  "nine": "9"
};

function parseReminder (text) {
  text = text.replace(/remind (me )?(to |about )?/, '');
  Object.keys(numberMap).forEach(function (num) {
    var numRegExp = new RegExp('\\b' + num + '\\b', 'g');
    text = text.replace(numRegExp, numberMap[num]);
  });

  var time;
  if (/\b(tomorrow|today|minute(s)?|hour(s)?|am|pm|noon|morning|afternoon|tonight|evening|midnight)\b/.test(text)) {
    time = date(text).toISOString()
  } else {
    return {
      error: 'Could not find a reminder time. Please try again.'
    }
  }

  var task = text.replace(rMeridiem, '');
  task = task.replace(rHourMinute, '');
  task = task.replace(rAtHour, '');
  task = task.replace(rDays, '');
  task = task.replace(rMonths, '');
  task = task.replace(rDayMod, '');
  task = task.replace(/\b(next|tomorrow|today|minute(s)?|hour(s)?)\b/g, '');
  task = task.replace(/\b(my)\b/g, 'your');

  if (task.length > 0) {
    return {
      datetime: time,
      task: capitalizeFirstLetter(task.trim())
    }
  } else {
    return {
      error: 'Could not find a task. Please try again.'
    }
  }
}
