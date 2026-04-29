const SHEET_ID = "YOUR_GOOGLE_SHEET_ID";
const ADMIN_EMAIL = "YOUR_ADMIN_EMAIL@example.com";

function doGet(e) {
  const sheet = SpreadsheetApp.openById(SHEET_ID).getActiveSheet();

  const uid = e.parameter.uid || "N/A";
  const name = e.parameter.name || "Unknown";
  const vehicle = e.parameter.vehicle || "Unknown";
  const action = e.parameter.action || "N/A";
  const userEmail = e.parameter.email || "";

  const now = new Date();

  const date = Utilities.formatDate(now, "Asia/Kolkata", "dd-MMM-yyyy");
  const time = Utilities.formatDate(now, "Asia/Kolkata", "hh:mm:ss a");

  sheet.appendRow([
    date,
    time,
    uid,
    name,
    vehicle,
    action
  ]);

  const adminSubject = "SmartPark RFID Alert - " + action;

  const adminBody =
    "SmartPark RFID Log\n\n" +
    "Action: " + action + "\n" +
    "Name: " + name + "\n" +
    "Vehicle No: " + vehicle + "\n" +
    "UID: " + uid + "\n" +
    "Date: " + date + "\n" +
    "Time: " + time;

  MailApp.sendEmail(ADMIN_EMAIL, adminSubject, adminBody);

  if (userEmail !== "") {
    const userSubject = "SmartPark Parking Update - " + action;

    const userBody =
      "Hello " + name + ",\n\n" +
      "Your vehicle (" + vehicle + ") has been recorded for: " + action + "\n\n" +
      "Date: " + date + "\n" +
      "Time: " + time + "\n\n" +
      "This is an automated SmartPark RFID notification.";

    MailApp.sendEmail(userEmail, userSubject, userBody);
  }

  return ContentService.createTextOutput("LOGGED SUCCESSFULLY");
}
