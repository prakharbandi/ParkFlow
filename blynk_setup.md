# Blynk Dashboard Setup

Create a Blynk Template and configure the following datastreams:

| Virtual Pin | Widget | Purpose |
|-------------|--------|---------|
| V0 | Value Display | Cars Parked |
| V1 | Value Display | Slots Available |
| V2 | Value Display | Total Slots |
| V3 | Label | Gate Status |
| V4 | Button | Manual Gate Control |

## Add Credentials in Code

Replace placeholders:

#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
