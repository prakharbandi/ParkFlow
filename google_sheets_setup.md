# Google Sheets Logging Setup

## Step 1: Create Google Sheet Columns

Date | Time | UID | Name | Vehicle No | Action

## Step 2: Deploy Apps Script

1. Open script.google.com
2. Paste smartpark_logger.gs code
3. Deploy as Web App

Deployment settings:

Execute as: Me
Who has access: Anyone

## Step 3: Paste Web App URL in ESP Code

String GOOGLE_SCRIPT_URL = "YOUR_GOOGLE_APPS_SCRIPT_WEB_APP_URL";

