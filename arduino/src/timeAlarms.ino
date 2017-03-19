// ******* initTimeAlarms ******* //
AlarmId wakeUpAlarm;
AlarmId prayerStartAlarm[N_PRAYER];
AlarmId prayerStopAlarm[N_PRAYER];
AlarmId timeSyncTimer;

void initTimeAlarms ()
{
	clearAlarms();

	timeSyncTimer = Alarm.timerRepeat (60, askForTime);
	print ("Timer nbr ");
	printNoPrefix (timeSyncTimer, DEC);
	printNoPrefix (" (Sync Timer) (");
	printNoPrefix (60, DEC);
	printNoPrefix (") : ");
	printNoPrefix (Alarm.read (timeSyncTimer), DEC);
	printlnNoPrefix();

	wakeUpAlarm = Alarm.alarmRepeat ((time_t) (WAKEUP_HOURS * 60 + WAKEUP_MINUTES) * 60, wakeUp);
	print ("Alarm nbr ");
	printNoPrefix (wakeUpAlarm, DEC);
	printNoPrefix (" (Wake up) (");
	printDigits (WAKEUP_HOURS);
	printNoPrefix (":");
	printDigits (WAKEUP_MINUTES);
	printNoPrefix (") (");
	printNoPrefix ((time_t) (WAKEUP_HOURS * 60 + WAKEUP_MINUTES) * 60, DEC);
	printNoPrefix (") : ");
	printNoPrefix (Alarm.read (wakeUpAlarm), DEC);
	printlnNoPrefix();

	for (int i = 0; i < N_PRAYER; i++)
	{
		prayerStartAlarm[i] = Alarm.alarmRepeat ((time_t) prayerTime[i][2] * 60, prayerStart);
		print ("Alarm nbr ");
		printNoPrefix (prayerStartAlarm[i], DEC);
		printNoPrefix (" (Salat ");
		printNoPrefix (prayersName[i]);
		printNoPrefix (" start) (");
		printDigits (prayerTime[i][0]);
		printNoPrefix (":");
		printDigits (prayerTime[i][1]);
		printNoPrefix (") (");
		printNoPrefix ((long) prayerTime[i][2] * 60, DEC);
		printNoPrefix (") : ");
		printNoPrefix (Alarm.read (prayerStartAlarm[i]), DEC);
		printlnNoPrefix();

		prayerStopAlarm[i] = Alarm.alarmRepeat ((time_t) prayerTime[i][2] * 60 + 10, prayerStop);
		print ("Alarm nbr ");
		printNoPrefix (prayerStopAlarm[i], DEC);
		printNoPrefix (" (Salat ");
		printNoPrefix (prayersName[i]);
		printNoPrefix (" end) (");
		printDigits (prayerTime[i][0]);
		printNoPrefix (":");
		printDigits (prayerTime[i][1] + 10);
		printNoPrefix (") (");
		printNoPrefix ((long) prayerTime[i][2] * 60 + 10, DEC);
		printNoPrefix (") : ");
		printNoPrefix (Alarm.read (prayerStopAlarm[i]), DEC);
		printlnNoPrefix();
	}

	println ("Alarms set\n");
} // initTimeAlarms

void clearAlarms ()
{
	boolean cleared = false;

	if (Alarm.isAllocated (timeSyncTimer))
	{
		Alarm.free (timeSyncTimer);
		cleared = true;
	}
	if (Alarm.isAllocated (wakeUpAlarm))
	{
		Alarm.free (wakeUpAlarm);
		cleared = true;
	}
	for (int i = 0; i < N_PRAYER; i++)
	{
		if (Alarm.isAllocated (prayerStartAlarm[i]))
		{
			Alarm.free (prayerStartAlarm[i]);
			cleared = true;
		}
		if (Alarm.isAllocated (prayerStopAlarm[i]))
		{
			Alarm.free (prayerStopAlarm[i]);
			cleared = true;
		}
	}

	if (cleared)
	{
		println ("Alarms cleared");
	}
	else
	{
		println ("Nothing to clear");
	}
} // clearAlarms

// Asking for time to the ESP8266 (via internet)
void askForTime ()
{
	println ("Gently asking for time\n");
	Serial1.print ("TIMEPLEASE");
}

// Action on Wake up alarm
void wakeUp ()
{
	mode = MODE_WAKEUP;
	on   = true;

	println ("Wake up !\n");
}

// Test wakeup time and peak hours for resynchronization
void prayerStart ()
{
	mode      = MODE_FADE;
	fadeSpeed = 97;
	on        = true;

	println ("Time to pray !\n");
	print ("It will stop at ");
	print (hour(), DEC);
	print (":");
	println (minute(), DEC);
}

void prayerStop ()
{
	mode = MODE_DEFAULT;
	on   = false;

	println ("Stop alert\n");
}
