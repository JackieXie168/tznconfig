/*
 * tznconfig.c
 *
 * Created by Jackie Xie on 2011-08-20.
 * Copyright 2011 Jackie Xie. All rights reserved.
 *
 */

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

typedef enum {Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday} Days;
typedef enum {First, Second, Third, Fourth, Fifth} Weeks;
typedef enum {January = 1, February, March, April, May, June, July, August, September, October, November, December} Months; 	

typedef struct {
	int hour;
	int minute;
	int second;
} Time;

typedef struct {
	Time time;
	Days day;
	Weeks week;
	Months month;
	int year;
	time_t ptime;
} datetime;

int years[13] = {-1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* ======================(function header)========================
Function Name:char* itos (){}
Description:to convert integer to string
Arguments:unsigned n
Return:a string
written by jackiexie
Date:  2007/07/10
================================================================*/
char tmpbuf[17];
char* itos (int n)
{
  int i=0,j;
  //char* s;
  //char* u;
  char s[17];
  //s= (char*) malloc(17);
  //u= (char*) malloc(17);
  
  do{
    s[i++]=(char)( n%10+48 );
    n-=n%10;
  }
  while((n/=10)>0);
  for (j=0;j<i;j++)
  tmpbuf[i-1-j]=s[j];

  tmpbuf[j]='\0';
  return tmpbuf;
}


/* ======================(function header)========================
Function Name: char *GetStrBetweenStr(char *,
                                char *,char *)
Description: retrieve a string from between left position
                 and right position string.
Arguments:char *s,char *lstr,char *rstr
Return: string
written by jackie xie
Date:   2007/07/10
================================================================*/
char *GetStrBetweenStr(char *s,char *lstr,char *rstr)
{
   char *p=s,*lp=s,*rp;
   do
   {
      /*trim(lstr);
      trim(rstr);*/
      lp=strstr(lp,lstr);
      if(lp)
      {
         rp=lstr;
         do lp++; while(*rp++);
         lp--;
         rp=strstr(lp,rstr);
         if(rp)
         {
            if(lp!=rp)
            {
               do *p++=*lp++; while(lp<rp);
               lp--;
            }
            rp=rstr;
            do lp++; while(*rp++);
            lp--;
         }
      }
   } while(lp);
   *p=0;
   return s;
}

/* ======================(function header)========================
Function Name: int matchStrPosAt (const char *, const char *)
Description: find the sub-string from the source string.
Arguments: const char *, const char *
Return: Returns "the array element number"(or "position") where 
	  "sub-string" matches before in "original string", or return 
	  "-1" if substr is not present in "original string".
written by jackie xie
Date   :  2007/07/10
================================================================*/
/* 
 *	delimiters <= n：return position before n-th matched substr.
 *	delimiters  = 0：do nothing.
 *	delimiters >= n：return position after n-th matched substr.
 */
int matchStrPosAt (char * substr, char * str, int delimiters)
{
	int i = -1, k, str_index, total = 0;
	int substr_len = strlen(substr), str_len = strlen(str);
	int matchpos = -1;
	int delimiter[str_len], ind_delim = 0;
	//int n = abs(delimiters);

	for(k=0; k<=str_len ; k++)
		delimiter[k] = -1;

	if (str == NULL || substr == NULL) return -1;

	/* substr has to be smaller than str */
	if (substr_len > str_len) return -1;

	/* look through str for substr, stopping at end of string or when the length
	 * of substr exceeds the remaining number of characters to be searched */ 
	while (str[++i] != '\0' && (str_len - i + 1) > substr_len) 
	{
		for (str_index = 0; str_index < substr_len; ++str_index)
		{
			/* definitely not at location i */
			if (str[i + str_index] != substr[str_index])
				break;
			/* if last letter matches, then we know the whole thing matched */
			else if (str_index == substr_len - 1)
			{
				if(delimiters >= 1)
				{
					i += substr_len -1;
					delimiter[++ind_delim] = i;
					//printf("delimiter[%d] = %d\n", ind_delim, delimiter[ind_delim]);
				}
				else if(delimiters == 0)
					return i;
				else if(delimiters == -1)
					total++;
			}
		}
	}

	if(delimiters == 0)
		return -1;						/* substr not present in str */
	else if(delimiters == -1)
		return total;					/* the total numbers of substr */
	else if(delimiters < ind_delim)
		matchpos = delimiter[delimiters];
	else
		matchpos = delimiter[ind_delim];

	return matchpos;
}

/* ======================(function header)========================
Function Name: int strcutail (char *str, const char *n, int pos)
Description: To remove the sub-string which is starting at 
			 n-th delimiter(or include it) to the end of input string.
Arguments: char *, const char *
Return: Returns the string which was cut the tailed sub-string off.
written by Jackie Xie
Date   :  2011/07/15
================================================================*/
char *strcutail (char *str, const char *n, int pos)
{
	int i, _newStrLen = matchStrPosAt(n, str, abs(pos));
	char* _new;
	
	if(_newStrLen >= 0){
		if (pos >= 0)
			++_newStrLen;			
		if ((_new = (char *)malloc(_newStrLen)) == NULL)
			return "NULL";

		(_new)[_newStrLen] = '\0';		

		/* copy sub-string from the head of the string to the specified delimiter */
		for (i = 0; i < _newStrLen; ++i)
			(_new)[i] = str[i];

		strcpy(str, _new);
		free (_new);
	}
	return str;
}

/* ======================(function header)========================
Function Name: int strmhead (char *str, const char *n, int pos)
Description: To cut the front of sub-string which is starting at the
			  matched delimiter and ending of the input string.
Arguments: char *, const char *
Return: The string which was cut the front of input string till the 
				specified delimiter.
written by Jackie Xie
Date   :  2011/08/12
================================================================*/
char *strmhead (char *str, const char *n, int pos)
{
	int i, _matchedStrLen = matchStrPosAt(n, str, abs(pos));
	int str_len = strlen(str);
	int _newStrLen = 0;
	
	char* _new;
	
	if(_matchedStrLen >= 0){
		if (pos >= 0)
			_matchedStrLen -= strlen(n);
			
		_newStrLen = str_len - _matchedStrLen;

		if ((_new = (char *)malloc(_newStrLen)) == NULL)
			return "NULL";

		(_new)[_newStrLen+1] = '\0';		

		/* copy sub-string from specified delimiter */
		for (i = 0; i < _newStrLen; ++i)
			(_new)[i] = str[_matchedStrLen +1 + i];
		strcpy(str, _new);
		free (_new);
	}
	
	return str;
}

bool isLeapYear (int year)
{
	if(((year % 4==0) && (year%100 != 0)) || (year % 400 == 0))
	   return true;
	else
		return false;	
}

bool IsLeapYear(int year)	
{					
	if(year%4==0) //必須能被4整除	
	{	
		 if(year%100==0)	
		 {	
				if(year%400==0)
					return	true;//如果以00結尾,還要能被400整除			 
				else
					return false;		
		 }
		 else
		 	return true;		
	}
	else
		return false;	
}	

/* ======================(function header)========================
Function Name : Days get_day_code (int year){}
Description : The codes are: day_code (0 = Sun, 1 = Mon, etc.).
Arguments : a year as a decimal number with century.
Return : an user defined type 'Days' value.
written by Jackie Xie
Date : 2011/08/28
================================================================*/
Days get_day_code (int year)
{
	int day_code;
	int x1, x2, x3;
	x1 = (year - 1.)/ 4.0;
	x2 = (year - 1.)/ 100.;
	x3 = (year - 1.)/ 400.;
	day_code = (year + x1 - x2 + x3) %7;
	return day_code;
}

/* ======================(function header)========================
Function Name : int dayOfMonth(datetime dt){}
Description : To convert the day(Sun...Sat) of week(1~5) of 
					month(1~12) into the day of the month as a decimal 
					number(01-31).
Arguments : a user defined type 'datetime' dt.
Return : an interger value which range is from 1 to 31.
written by Jackie Xie
Date : 2011/08/28
================================================================*/
int dayOfMonth(datetime dt)
{
	int days, month;
	Days day_code;

	if(isLeapYear(dt.year))
		years[February] = 29;
	
	//printf("February have %d days in %d\n", years[February], dt.year);
	
	day_code = get_day_code(dt.year);
	for ( month = 1; month < dt.month; month++ )
	{
		/* set day_code for next month to begin */
		day_code = ( day_code + years[month] ) % 7;	
	}
	
	days = (dt.day >= day_code)?(dt.day - day_code + 1)+7*(dt.week-1): (7 - day_code + 1) + dt.day + 7*(dt.week-1);
	
	while(days > years[dt.month])
		days -= 7;
	
	return days;
}

/* ======================(function header)========================
Function Name :t ime_t str2datetime(char *datetime){}
Description : To Convert the string of datetime format into an interger 
					which unit is a time 'second'.
Arguments : char * tz
Return : an integer of type 'time_t '
written by Jackie Xie
Date : 2011/08/29
================================================================*/
time_t str2datetime(char *datetime)
{
	int year, month, day, hour, min, sec;
	struct tm *when;
	time_t tme;
	char tmp_time[32];
	strcpy(tmp_time, datetime);

	sscanf(tmp_time, "%4d-%02d-%02dT%02d:%02d:%02dZ", &year, &month, &day, &hour, &min, &sec);

	time(&tme);
	when = localtime(&tme);
	when->tm_year = year - 1900;
	when->tm_mon = month-1;
	when->tm_mday = day;
	when->tm_hour = hour;
	when->tm_min = min;
	when->tm_sec = sec;

	return(mktime(when));
}

/* ======================(function header)========================
Function Name:int getTZ(char **tz){}
Description : To find the time difference[Coordinated Universal Time
			(UTC)] between GMT and Local time zone.
Arguments : char ** tz
Return : a string with format "[+|-]hh:mm"
written by jackiexie
Date : 2011/06/02
Example :
	int main(int argc, char** argv)
	{
		char *tz;
	
		getTZ(&tz);
		printf("UTC%s", tz);
	
		return 0;
	}
================================================================*/
int getTZ(char **tz)
{
	struct tm *tptr;
	char UTC[20];
	time_t secs,l_secs,gmt_secs;
	float utc;
	char *min, *sign;
	int tmp;
	
	time(&secs);
	tptr = localtime(&secs);
	l_secs = mktime(tptr);
	tptr = gmtime(&secs);
	gmt_secs = mktime(tptr);
	utc = l_secs - gmt_secs;
	
	utc = utc/3600.0;
	//utc = -9.9;//-2.5;-3.5;-4.5;-9.5;-12;-10;5.75;8.75;12.75;13.75;14

	if(utc < 0)
		sign = strdup("-");
	else if(utc > 0)
		sign = strdup("+");

	tmp = utc * 60;
	tmp = abs(tmp) % 60;
	
	if(tmp == 30)
		min = strdup("30");
	else if(tmp == 45)
		min = strdup("45");
	else if(tmp != 0)
		min = strdup(itos(tmp));
	else
		min = strdup("00");

	tmp = (int)abs(utc);
	if(tmp <= 14 && tmp >= 10)
		sprintf(UTC, "%s%d:%s", sign, tmp, min);
	else if(tmp < 10 && tmp > 0)
		sprintf(UTC, "%s0%d:%s", sign, tmp, min);
	else
		sprintf(UTC, "00:00");

	*tz = strdup(UTC);
	//printf("%s(%s)\n", UTC, *tz);
	if(*tz == NULL)
		return -1;
	
	free(min);
	free(sign);
	
	return 0;
}

/* ======================(function header)========================
Function Name:int setTZ(char **tz){}
Description : To Setup the system date、time by timezone information.
Arguments : char * tz
Return : a string with format "[+|-]hh:mm"
written by Jackie Xie
Date : 2011/08/18
================================================================*/
int setTZ(char *timezone)
{
	struct tm *tptr;
	struct timeval tv;
	struct timezone tz;
	time_t secs;
	int utc = 0;
	int hsecs, msecs;
	int rc;
	char *sHour = strdup(timezone);
	char *sMin = strdup(timezone);

	(void) time(&secs);
	tptr = localtime(&secs);
	printf("The timezone is 'UTC%s'\n", timezone);
	printf("The current date/time :\n");
	printf("        Date : %02d-%02d-%02d\n", 1900+tptr->tm_year, tptr->tm_mon+1, tptr->tm_mday);
	printf("        Time : %02d:%02d:%02d\n\n", tptr->tm_hour, tptr->tm_min, tptr->tm_sec);

	if(strstr(timezone,":")){
		hsecs = atoi(strcutail(sHour, ":", -100)) * 3600;
		msecs = atoi(strmhead(sMin, ":", -100)) * 60;
	}
	else{
		hsecs = atoi(sHour) * 3600;
		msecs = 0;
	}	
	
	utc = hsecs + msecs;
	 (void) time(&secs);
	
	if(utc==0)
		tptr = localtime(&secs);
	else{
		secs += utc;
		tptr = gmtime(&secs);
	}

	// Make new system time.
	if ((tv.tv_sec = mktime(tptr)) == (time_t)-1)
	{
		printf("Cannot convert system time\n");
	}

	tz.tz_minuteswest = -(secs / 60);	
	tv.tv_usec = 0;
	rc = settimeofday(&tv, &tz);
	if ( rc != 0)
	{
		printf("Cannot set system time\n");
		printf("settimeofday() failed, " "errno = %d\n",errno);
		return -1;
	}
	else
		printf("settimeofday() successful.\n");

	printf("Update timezone:'UTC%s'\n", timezone);
	printf("The new date/time is :\n");
	printf("        Date: %02d-%02d-%02d\n", 1900+tptr->tm_year, tptr->tm_mon+1, tptr->tm_mday);
	printf("        Time: %02d:%02d:%02d\n\n", tptr->tm_hour, tptr->tm_min, tptr->tm_sec);

	free(sMin);
	free(sHour);
	
	return 0;
}

/* ======================(function header)===========================
Function Name:int setTZName(char *tzname){}
Description : To Setup the system date、time by a specified local timezone name which 
					is encoded according to IEEE 1003.1 (POSIX).
					
					Specifying the Time Zone with TZ
					
					In POSIX systems, a user can specify the time zone by means of the TZ environment variable. For information about how to set environment variables, see section Environment Variables. The functions for accessing the time zone are declared in `time.h'.
					
					You should not normally need to set TZ. If the system is configured properly, the default time zone will be correct. You might set TZ if you are using a computer over a network from a different time zone, and would like times reported to you in the time zone local to you, rather than what is local to the computer.
					
					In POSIX.1 systems the value of the TZ variable can be in one of three formats. With the GNU C library, the most common format is the last one, which can specify a selection from a large database of time zone information for many regions of the world. The first two formats are used to describe the time zone information directly, which is both more cumbersome and less precise. But the POSIX.1 standard only specifies the details of the first two formats, so it is good to be familiar with them in case you come across a POSIX.1 system that doesn't support a time zone information database.
					
					The first format is used when there is no Daylight Saving Time (or summer time) in the local time zone:
					
					std offset
					The std string specifies the name of the time zone. It must be three or more characters long and must not contain a leading colon, embedded digits, commas, nor plus and minus signs. There is no space character separating the time zone name from the offset, so these restrictions are necessary to parse the specification correctly.
					
					The offset specifies the time value you must add to the local time to get a Coordinated Universal Time value. It has syntax like [+|-]hh[:mm[:ss]]. This is positive if the local time zone is west of the Prime Meridian and negative if it is east. The hour must be between 0 and 23, and the minute and seconds between 0 and 59.
					
					The minutes ( mm) and seconds ( ss) are optional. The hour ( hh) shall be required and may be a single digit. The offset following std shall be required. If no offset follows dst, the alternative time is assumed to be one hour ahead of standard time. One or more digits may be used; the value is always interpreted as a decimal number. The hour shall be between zero and 24, and the minutes (and seconds)-if present-between zero and 59. The result of using values outside of this range is unspecified. If preceded by a '-', the timezone shall be east of the Prime Meridian; otherwise, it shall be west (which may be indicated by an optional preceding '+' ).
					
					For example, here is how we would specify Eastern Standard Time, but without any Daylight Saving Time alternative:
					
					EST+5
					The second format is used when there is Daylight Saving Time:
					
					std offset dst [offset],start[/time],end[/time]
					The initial std and offset specify the standard time zone, as described above. The dst string and offset specify the name and offset for the corresponding Daylight Saving Time zone; if the offset is omitted, it defaults to one hour ahead of standard time.
					
					The remainder of the specification describes when Daylight Saving Time is in effect. The start field is when Daylight Saving Time goes into effect and the end field is when the change is made back to standard time. The following formats are recognized for these fields:
					
					Jn
					This specifies the Julian day, with n between 1 and 365. February 29 is never counted, even in leap years.
					n
					This specifies the Julian day, with n between 0 and 365. February 29 is counted in leap years.
					
					Mm.w.d
					This specifies day d of week w of month m. The day d must be between 0 (Sunday) and 6. The week w must be between 1 and 5; week 1 is the first week in which day d occurs, and week 5 specifies the last d day in the month. The month m should be between 1 and 12.
					The time fields specify when, in the local time currently in effect, the change to the other time occurs. If omitted, the default is 02:00:00.
					
					For example, here is how you would specify the Eastern time zone in the United States, including the appropriate Daylight Saving Time and its dates of applicability. The normal offset from UTC is 5 hours; since this is west of the prime meridian, the sign is positive. Summer time begins on the first Sunday in April at 2:00am, and ends on the last Sunday in October at 2:00am.
					
					EST+5EDT,M4.1.0/2,M10.5.0/2

Arguments : char * tz
Return : a string with format "std offset dst [offset],start[/time],end[/time]"
written by Jackie Xie
Date : 2011/08/28
===================================================================*/
int setTZName(char *tzname)
{
	char *offsets = strdup(tzname);
	char stz[10]={0};
	char dtz[10]={0};
	char stz_offset[10]={0};
	char dtz_offset[10]={0};
	char *start_dst = NULL;
	char *end_dst = NULL;
	int len, i = -1, chstr = 0, chnum = 0;
	int stdlen = 0, dstlen = 0, sofflen = 0, dofflen = 0;
	char *times, *day, *week, *month, *year, *tmp;
	datetime currtime, sdst, edst;
	time_t ptime = 0;
	char data[32];
	
	sdst.ptime = edst.ptime = 0;

	if(strstr(tzname, ","))
	{
		time_t currTime;
		struct tm* pLocalTime=NULL;
	
		times = strdup("00:00:00");
		day = strdup("00");
		week = strdup("0");
		month = strdup("00");
		year = strdup("0000");

		currTime = time(NULL);
		pLocalTime = localtime(&currTime);
		strftime(times, 10 ,"%H:%M:%S", pLocalTime);
		strftime(day, 3 ,"%d", pLocalTime);
		strftime(week, 2 ,"%w", pLocalTime);
		strftime(month, 3 ,"%m", pLocalTime);
		strftime(year, 5, "%Y", pLocalTime);
		
		tmp = strdup(times); currtime.time.hour = atoi(strcutail(tmp, ":", -1));
		tmp = strdup(times); currtime.time.minute = atoi(GetStrBetweenStr(tmp, ":", ":"));
		tmp = strdup(times); currtime.time.second = atoi(strmhead(tmp, ":", -2));
		currtime.day = atoi(day);
		currtime.week = atoi(week);
		currtime.month = atoi(month);
		currtime.year = atoi(year);
		printf("current time is '%4d-%02d-%02dT%02d:%02d:%02dZ'\n", currtime.year, currtime.month, currtime.day, currtime.time.hour, currtime.time.minute, currtime.time.second);

		start_dst = strdup(tzname);
		end_dst = strdup(tzname);
		tmp = strdup(tzname);
		strcutail(offsets, ",", -1);
		//printf("offsets = %s\n", offsets);
		start_dst = strdup(GetStrBetweenStr(tmp, ",", ","));
		if(!strstr(start_dst, "/"))
			sprintf(start_dst, "%s/2", start_dst);
		
		times = strdup(start_dst);
		day = strdup(start_dst);
		week = strdup(start_dst);
		month = strdup(start_dst);
		
		//sdst.time = atoi(strmhead(times, "/", -1));
		tmp = strdup(strmhead(times, "/", -1));
		if(strstr(tmp, ":"))
		{
			times = strdup(tmp); sdst.time.hour = atoi(strcutail(times, ":", -1));
			times = strdup(tmp); printf("times : %s\n", times);
			if(strstr(strmhead(times, ":", -1), ":")) sdst.time.minute = atoi(strcutail(times, ":", -1));
			else sdst.time.minute = atoi(times);
			times = strdup(tmp);
			if(strstr(strmhead(times, ":", -1), ":")) sdst.time.second = atoi(strmhead(times, ":", -1));
			else sdst.time.second = 0;
		}
		else
		{
			sdst.time.hour = atoi(tmp);
			sdst.time.minute = 0;
			sdst.time.second = 0;
		}
			
		sdst.day = atoi(strcutail(strmhead(day, ".", -2), "/", -1));
		sdst.week = atoi(GetStrBetweenStr(week, ".", "."));
		sdst.month = atoi(GetStrBetweenStr(month, "M", "."));

		strmhead(end_dst, ",", -2);
		if(!strstr(end_dst, "/"))
			sprintf(end_dst, "%s/2", end_dst);
		
		times = strdup(end_dst);
		day = strdup(end_dst);
		week = strdup(end_dst);
		month = strdup(end_dst);
		
		//edst.time = atoi(strmhead(times, "/", -1));
		tmp = strdup(strmhead(times, "/", -1));
		if(strstr(tmp, ":"))
		{
			times = strdup(tmp); edst.time.hour = atoi(strcutail(times, ":", -1));
			times = strdup(tmp);
			if(strstr(strmhead(times, ":", -1), ":")) edst.time.minute = atoi(strcutail(times, ":", -1));
			else edst.time.minute = atoi(times);
			times = strdup(tmp);
			if(strstr(strmhead(times, ":", -1), ":")) edst.time.second = atoi(strmhead(times, ":", -1));
			else edst.time.second = 0;
		}
		else
		{
			edst.time.hour = atoi(tmp);
			edst.time.minute = 0;
			edst.time.second = 0;
		}

		edst.day = atoi(strcutail(strmhead(day, ".", -2), "/", -1));
		edst.week = atoi(GetStrBetweenStr(week, ".", "."));
		edst.month = atoi(GetStrBetweenStr(month, "M", "."));
		
		/* To confirm the year for start and end of the daylight saving time */
		if(sdst.month > edst.month)
		{
			if(currtime.month >= sdst.month){
				sdst.year = currtime.year;
				edst.year = currtime.year + 1;
			}
			else if(currtime.month <= edst.month){
				sdst.year = currtime.year - 1;
				edst.year = currtime.year;
			}
			else
				sdst.year = edst.year = currtime.year;
		}
		else if(sdst.month == edst.month)
		{
			if(sdst.week > edst.week || (sdst.week == edst.week && (sdst.day > edst.day || 
				(sdst.day == edst.day && (sdst.time.hour > edst.time.hour || 
				(sdst.time.hour == edst.time.hour && (sdst.time.minute > edst.time.minute ||
				(sdst.time.minute == edst.time.minute && sdst.time.second > edst.time.second))))))))
			{
				sdst.year = currtime.year;
				edst.year = currtime.year + 1;
			}
			else
				sdst.year = edst.year = currtime.year;				
		}
		else
			sdst.year = edst.year = currtime.year;

		printf("start daylight saving time is '%4d-%02d-%02dT%02d:%02d:%02dZ'\n", sdst.year, sdst.month, dayOfMonth(sdst), sdst.time.hour, sdst.time.minute, sdst.time.second);
		printf("end daylight saving time is '%4d-%02d-%02dT%02d:%02d:%02dZ'\n", edst.year, edst.month, dayOfMonth(edst), edst.time.hour, edst.time.minute, edst.time.second);
		
		sprintf(data, "%4d-%02d-%02dT%02d:%02d:%02dZ\n", currtime.year, currtime.month,  currtime.day, currtime.time.hour, currtime.time.minute, currtime.time.second);
		currtime.ptime = str2datetime(data);
		//printf("%d\n", (int)currtime.ptime);
		ptime = currtime.ptime;

		sprintf(data, "%4d-%02d-%02dT%02d:%02d:%02dZ\n", sdst.year, sdst.month,  sdst.day, sdst.time.hour, sdst.time.minute, sdst.time.second);
		sdst.ptime = str2datetime(data);
		//printf("%d\n", (int)sdst.ptime);

		sprintf(data, "%4d-%02d-%02dT%02d:%02d:%02dZ\n", edst.year, edst.month,  edst.day, edst.time.hour, edst.time.minute, edst.time.second);
		edst.ptime = str2datetime(data);
		//printf("%d\n", (int)edst.ptime);
		
		free(times); free(tmp);
	}

	len = strlen(offsets);
	
	chstr = 0; chnum = 0;
	while (offsets[++i] != '\0')
	{
		offsets[stdlen] = toupper(offsets[stdlen]);
		if(isalpha(offsets[i]))
		{
			if(!chstr)
			{
				stz[stdlen] = offsets[i];
				stdlen++ ;
			}
			else
			{
				chnum = 1;
				dtz[dstlen] = offsets[i];
				dstlen++ ;
			}
		}
		else if(isdigit(offsets[i]) || offsets[i] == '+' || offsets[i] == '-' || offsets[i] == ':')
		{
			/* If preceded by a '-', the timezone shall be east of the Prime Meridian; 
			 * otherwise, it shall be west (which may be indicated by an optional preceding '+' ).
			 * Note: The +,- definition specifies east and west and not addition or subtraction, 
			 * so be careful. (UTC+1 translates to <Timezone name>-1) 
			 */
			chstr = 1;
			if(!chnum)
			{
				stz_offset[sofflen] = offsets[i];
				sofflen++ ;
			}
			else
			{
				dtz_offset[dofflen] = offsets[i];
				dofflen++ ;
			}
		}
	}
	printf("Standard time zone is '%s%s'.\n", stz, stz_offset);

	{	
		char *shour = strdup(stz_offset);
		char *sminute = strdup(stz_offset);
		int doffset = 0, stzhour = atoi(shour) * (-1);

		if(strstr(stz, "GMT") || strstr(stz, "UTC") || !strcmp(stz, ""))
			stzhour = atoi(shour);
		else
			stzhour = atoi(shour) * (-1);

		if(dstlen && !dofflen)
			doffset = stzhour + 1; 
		else doffset = stzhour;
		
		if(strstr(stz_offset, ":"))
		{
			strcutail(shour, ":", -1);
			strmhead(sminute, ":", -1);
			if(doffset > 0)
				sprintf(dtz_offset, "+%d:%s", doffset, sminute);
			else
				sprintf(dtz_offset, "%d:%s", doffset, sminute);
			if(stzhour > 0)
				sprintf(stz_offset, "+%d:%s", stzhour, sminute);
			else
				sprintf(stz_offset, "%d:%s", stzhour, sminute);
		}
		else
		{
			if(doffset > 0)
				sprintf(dtz_offset, "+%d", doffset);
			else
				sprintf(dtz_offset, "%d", doffset);
			if(stzhour > 0)
				sprintf(stz_offset, "+%d", stzhour);
			else
				sprintf(stz_offset, "%d", stzhour);
		}
		free(shour); free(sminute);
	}

	if(dstlen) printf("Daylight Saving Time zone is '%s%s'.\n\n", dtz, dtz_offset);
	
	/* improve accuracy in timezone configuration */
	bool duringTheDST = (currtime.year > sdst.year && (currtime.year == edst.year && 
		(currtime.month < edst.month || (currtime.month == edst.month &&
			(currtime.day < dayOfMonth(edst) || (currtime.day == dayOfMonth(edst) && 
				(currtime.time.hour < edst.time.hour || (currtime.time.hour == edst.time.hour && 
					(currtime.time.minute < edst.time.minute || (currtime.time.minute == edst.time.minute && currtime.time.second < edst.time.second)))))))))) ||
		(currtime.year == sdst.year && 
			(currtime.month > sdst.month || (currtime.month == sdst.month && 
				(currtime.day > dayOfMonth(sdst) || (currtime.day == dayOfMonth(sdst) && 
					(currtime.time.hour > sdst.time.hour || (currtime.time.hour == sdst.time.hour && (currtime.time.minute > sdst.time.minute || 
						(currtime.time.minute == sdst.time.minute && currtime.time.second > sdst.time.second)))))))) &&
		(currtime.year < edst.year || (currtime.year == edst.year &&
			(currtime.month < edst.month || (currtime.month == edst.month && 
				(currtime.day < dayOfMonth(edst) || (currtime.day == dayOfMonth(edst) && 
					(currtime.time.hour < edst.time.hour) || (currtime.time.hour == edst.time.hour && (currtime.time.minute < edst.time.minute || 
						(currtime.time.minute == edst.time.minute && currtime.time.second < edst.time.second))))))))));
	if(duringTheDST)
	/* if(ptime >= sdst.ptime && ptime <= edst.ptime) */
	/*if((currtime.year > sdst.year && currtime.year == edst.year && currtime.day <= dayOfMonth(edst)) ||
		(currtime.year == sdst.year && currtime.month >= sdst.month && currtime.day >= dayOfMonth(sdst))) */
		setTZ(dtz_offset);
	else
		setTZ(stz_offset);

	free(offsets); free(start_dst); free(end_dst);
	return 0;
}

int main(int argc, char *argv[])
{
	//struct timeval tv;
	char help_msg[1024] = "Usage: tznconfig [-s <timezone>] [-x <timezone name is encoded according to IEEE 1003.1 (POSIX)>] [-i] [-h]\n\n";
	char *tz, *tzname;
	int c;
	
	strcat(help_msg, "tznconfig command summary\n");
	strcat(help_msg, "\ttznconfig is a function to setup/get timezone infomation.\n");
	strcat(help_msg, "\t-s：setup datetime for specified timezone.\n");
	strcat(help_msg, "\t\t<timezone>：input format：'<+ | ->xx:xx' (x=[0~9]).\n");
	strcat(help_msg, "\t-x：setup datetime for specified timezone name.\n");
	strcat(help_msg, "\t\t<timezone name>：input format：'std offset dst [offset],start[/time],end[/time]' .\n");
	strcat(help_msg, "\t-i：To get te local timezone information.\n");
	strcat(help_msg, "\t-h：To show this help message.\n");

	if(argc <= 1 || ((isgraph(*argv[1]) || ispunct(*argv[1])) && *argv[1]!='-'))
		fprintf(stderr, "%s", help_msg);

	while ((c = getopt(argc, argv, "s:x:i0:h0:?0:0:-")) != -1){
		switch (c) {
			case 's':
				tz = optarg;
				setTZ(tz);
				break;
			case 'x':
				tzname = optarg;
				setTZName(tzname);
				break;
			case 'i':
				getTZ(&tz);
				printf("本地時區是：UTC%s\n", tz);
				break;
			case 'h':
			case '?':
			default:
				fprintf(stderr, "%s", help_msg);
				exit(0);
				break;
		}
	}

	return 0;
}
