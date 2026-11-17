/* SPDX-License-Identifier: MIT-0 */
#ifndef CTB_DATE_H
#define CTB_DATE_H
#include "io.h"
/* Proleptic Gregorian calendar, years 1..9999, no timezone. */
typedef struct {int year,month,day;} Date;
typedef struct {bool ok;Date value;} DateResult;
/* Durations are nonnegative and are not limited to 24 hours. */
typedef struct {long long seconds;} Duration;
typedef struct {long long hours;int minutes,seconds;} DurationParts;
static Duration duration_from_hms(long long hours,int minutes,int seconds){
    if(hours<0||minutes<0||minutes>59||seconds<0||seconds>59)tb_fail("invalid duration");
    if(hours>LLONG_MAX/3600)tb_fail("duration overflow");long long total=hours*3600;
    if(total>LLONG_MAX-minutes*60LL-seconds)tb_fail("duration overflow");return (Duration){total};
}
static DurationParts duration_to_hms(Duration d){if(d.seconds<0)tb_fail("negative duration");return (DurationParts){d.seconds/3600,(int)(d.seconds/60%60),(int)(d.seconds%60)};}
static Duration duration_add(Duration a,Duration b){if(a.seconds<0||b.seconds<0||a.seconds>LLONG_MAX-b.seconds)tb_fail("duration overflow");return (Duration){a.seconds+b.seconds};}
static Duration read_duration(void){return duration_from_hms(read_ll(),read_int(),read_int());}
static void print_duration(Duration d,const char *end){DurationParts p=duration_to_hms(d);print_format("%lld:%02d:%02d",end,p.hours,p.minutes,p.seconds);}
static bool date_is_leap(int year){return year%4==0 && (year%100!=0 || year%400==0);}
static int date_month_days(int year,int month){static const int days[]={31,28,31,30,31,30,31,31,30,31,30,31};if(year<1 || year>9999 || month<1 || month>12)tb_fail("invalid year/month");return days[month-1]+(month==2 && date_is_leap(year));}
static bool date_valid(Date d){return d.year>=1 && d.year<=9999 && d.month>=1 && d.month<=12 && d.day>=1 && d.day<=date_month_days(d.year,d.month);}
/* Returns failure if the original day does not exist in the requested month. */
static DateResult date_in_year_month(Date d,int year,int month){Date candidate={year,month,d.day};return (DateResult){date_valid(d)&&date_valid(candidate),candidate};}
static DateResult try_date(int year,int month,int day){Date d={year,month,day};return (DateResult){date_valid(d),d};}
static Date date_new(int year,int month,int day){DateResult r=try_date(year,month,day);if(!r.ok)tb_fail("invalid date");return r.value;}
static int tb_year_days(int year){int y=year-1;return y*365+y/4-y/100+y/400;}
static int date_ordinal(Date d){if(!date_valid(d))tb_fail("invalid date");int n=tb_year_days(d.year)+d.day-1;for(int m=1;m<d.month;++m)n+=date_month_days(d.year,m);return n;}
static int date_compare(Date a,Date b){int x=date_ordinal(a),y=date_ordinal(b);return (x>y)-(x<y);}
/* Signed number of days from a to b. */
static int date_difference(Date a,Date b){return date_ordinal(b)-date_ordinal(a);}
static DateResult try_date_add_days(Date d,long long delta){int n=date_ordinal(d),limit=tb_year_days(10000);if(delta < -(long long)n || delta>(long long)(limit-1-n))return (DateResult){false,{0}};n+=(int)delta;int lo=1,hi=10000;while(lo+1<hi){int mid=lo+(hi-lo)/2;if(tb_year_days(mid)<=n)lo=mid;else hi=mid;}n-=tb_year_days(lo);int month=1;while(n>=date_month_days(lo,month))n-=date_month_days(lo,month++);return (DateResult){true,{lo,month,n+1}};}
static Date date_add_days(Date d,long long delta){DateResult r=try_date_add_days(d,delta);if(!r.ok)tb_fail("date outside years 1..9999");return r.value;}
/* Input is three whitespace-separated integers; output YYYY-MM-DD. */
static Date read_date(void){int y=read_int(),m=read_int(),d=read_int();return date_new(y,m,d);}
static void print_date(Date d,const char *end){if(!date_valid(d))tb_fail("invalid date");print_format("%04d-%02d-%02d",end,d.year,d.month,d.day);}
typedef struct {int hour,minute,second;} TimeOfDay;
static TimeOfDay time_from_seconds(int seconds){if(seconds<0 || seconds>=86400)tb_fail("time outside day");return (TimeOfDay){seconds/3600,seconds/60%60,seconds%60};}
static int time_to_seconds(TimeOfDay t){if(t.hour<0||t.hour>23||t.minute<0||t.minute>59||t.second<0||t.second>59)tb_fail("invalid time");return t.hour*3600+t.minute*60+t.second;}
static int time_difference(TimeOfDay a,TimeOfDay b){return time_to_seconds(b)-time_to_seconds(a);}
static TimeOfDay read_time(void){int h=read_int(),m=read_int(),s=read_int();TimeOfDay t={h,m,s};(void)time_to_seconds(t);return t;}
static void print_time(TimeOfDay t,const char *end){(void)time_to_seconds(t);print_format("%02d:%02d:%02d",end,t.hour,t.minute,t.second);}
#endif
