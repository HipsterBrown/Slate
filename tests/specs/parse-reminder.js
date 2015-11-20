var test = require('tape')

var date = require('date.js')
var parseReminder = require('../../src/js/parse-reminder')

test('Error when no time is found', function (t) {
  t.plan(1)

  var reminder = parseReminder('something not a reminder')

  t.true(reminder.error, 'returns an error property')
})

test('Error when no task is found', function (t) {
  t.plan(1)

  var reminder = parseReminder('remind me at noon')

  t.true(reminder.error, 'returns an error property')
})

test('Successful reminder at time', function (t) {
  t.plan(3)

  var time = '4:20 pm'
  var datetime = date(time).toTimeString()
  var task = 'Pick up the kids'

  var reminder = parseReminder('remind me to ' + task + ' at ' + time)

  t.is(typeof reminder, 'object', 'returns an object')
  t.is(new Date(reminder.datetime).toTimeString(), datetime, 'object has correct datetime property')
  t.is(reminder.task, task, 'object has correct task property')
})

test('Reminder at noon', function (t) {
  t.plan(3)

  var time = 'noon'
  var datetime = date('12:00 pm').toTimeString()
  var task = 'Grab lunch'

  var reminder = parseReminder('remind me to ' + task + ' at ' + time)

  t.is(typeof reminder, 'object', 'returns an object')
  t.is(new Date(reminder.datetime).toTimeString(), datetime, 'object has correct datetime property')
  t.is(reminder.task, task, 'object has correct task property')
})

test('Reminder this evening', function (t) {
  t.plan(3)

  var time = 'evening'
  var datetime = date('5:00 pm').toTimeString()
  var task = 'Leave work'

  var reminder = parseReminder('remind me to ' + task + ' this ' + time)

  t.is(typeof reminder, 'object', 'returns an object')
  t.is(new Date(reminder.datetime).toTimeString(), datetime, 'object has correct datetime property')
  t.is(reminder.task, task, 'object has correct task property')
})

test('Reminder this afternoon', function (t) {
  t.plan(3)

  var time = 'afternoon'
  var datetime = date('2:00 pm').toTimeString()
  var task = 'Go to meeting'

  var reminder = parseReminder('remind me to ' + task + ' this ' + time)

  t.is(typeof reminder, 'object', 'returns an object')
  t.is(new Date(reminder.datetime).toTimeString(), datetime, 'object has correct datetime property')
  t.is(reminder.task, task, 'object has correct task property')
})

test('Reminder at midnight', function (t) {
  t.plan(3)

  var time = 'midnight'
  var datetime = date('12:00 am').toTimeString()
  var task = 'Go to bed'

  var reminder = parseReminder('remind me to ' + task + ' at ' + time)

  t.is(typeof reminder, 'object', 'returns an object')
  t.is(new Date(reminder.datetime).toTimeString(), datetime, 'object has correct datetime property')
  t.is(reminder.task, task, 'object has correct task property')
})

test('Reminder this morning', function (t) {
  t.plan(3)

  var time = 'morning'
  var datetime = date('8:00 am').toTimeString()
  var task = 'Wake up'

  var reminder = parseReminder('remind me to ' + task + ' this ' + time)

  t.is(typeof reminder, 'object', 'returns an object')
  t.is(new Date(reminder.datetime).toTimeString(), datetime, 'object has correct datetime property')
  t.is(reminder.task, task, 'object has correct task property')
})

test('Reminder tomorrow morning', function (t) {
  t.plan(4)

  var time = 'tomorrow morning'
  var timeString = date(time).toTimeString()
  var dateString = date(time).toDateString()
  var task = 'Wake up now'

  var reminder = parseReminder('remind me to ' + task + ' ' + time)

  t.is(typeof reminder, 'object', 'returns an object')
  t.is(new Date(reminder.datetime).toTimeString(), timeString, 'object has correct datetime property')
  t.is(new Date(reminder.datetime).toDateString(), dateString, 'object has correct datetime property')
  t.is(reminder.task, task, 'object has correct task property')
})

test('Reminder Saturday at noon', function (t) {
  t.plan(4)

  var time = 'saturday at noon'
  var timeString = date(time).toTimeString()
  var dateString = date(time).toDateString()
  var task = 'Wake up now'

  var reminder = parseReminder('remind me to ' + task + ' on ' + time)

  t.is(typeof reminder, 'object', 'returns an object')
  t.is(new Date(reminder.datetime).toTimeString(), timeString, 'object has correct datetime property')
  t.is(new Date(reminder.datetime).toDateString(), dateString, 'object has correct datetime property')
  t.is(reminder.task, task, 'object has correct task property')
})

test('Reminder next Wednesday at 1:00 pm', function (t) {
  t.plan(4)

  var time = 'next wednesday at 1:00 pm'
  var timeString = date(time).toTimeString()
  var dateString = date(time).toDateString()
  var task = 'Clean up'

  var reminder = parseReminder('remind me to ' + task + ' ' + time)

  t.is(typeof reminder, 'object', 'returns an object')
  t.is(new Date(reminder.datetime).toTimeString(), timeString, 'object has correct datetime property')
  t.is(new Date(reminder.datetime).toDateString(), dateString, 'object has correct datetime property')
  t.is(reminder.task, task, 'object has correct task property')
})

test('Reminder December 9th at 1:00 pm', function (t) {
  t.plan(4)

  var time = '9th of december at 8:00 pm'
  var timeString = date(time).toTimeString()
  var dateString = date(time).toDateString()
  var task = 'Sing happy birthday'

  var reminder = parseReminder('remind me to ' + task + ' on ' + time)

  t.is(typeof reminder, 'object', 'returns an object')
  t.is(new Date(reminder.datetime).toTimeString(), timeString, 'object has correct datetime property')
  t.is(new Date(reminder.datetime).toDateString(), dateString, 'object has correct datetime property')
  t.is(reminder.task, task, 'object has correct task property')
})

test('Reminder task at some place', function (t) {
  t.plan(4)

  var time = 'saturday at noon'
  var timeString = date(time).toTimeString()
  var dateString = date(time).toDateString()
  var task = 'Pick up lunch at the market'

  var reminder = parseReminder('remind me to ' + task + ' on ' + time)

  t.is(typeof reminder, 'object', 'returns an object')
  t.is(new Date(reminder.datetime).toTimeString(), timeString, 'object has correct datetime property')
  t.is(new Date(reminder.datetime).toDateString(), dateString, 'object has correct datetime property')
  t.is(reminder.task, task, 'object has correct task property')
})
