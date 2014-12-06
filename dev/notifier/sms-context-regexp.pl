#!/usr/bin/perl

my $units = 'E17 E16 E30 TK17 4032';
my $UnitArray = split ' ', $units;
my $BoxArea = '317';
my $CallType = 'FIRECOM';
my $CallGroup = 'FIRE';

my $sql = "SELECT
    t2.ObjId,
    t2.MemberName,
    t2.NotifyMethod,
    t2.NotifyAddr,
    t2.ScheduleTimeOfDay,
    t2.ScheduleDayOfWeek
FROM NotifyRules t1
LEFT JOIN NotifyMember t2 ON t2.ObjId = t1.MemberObjId
WHERE " .
( $Type == 1 ? 
    "t1.NotifyOnDispatch = 1 AND " : "t1.NotifyOnActive = 1 AND "
) . "
(
    ( ? REGEXP t1.NotifyArea AND ? REGEXP t1.NotifyRuleset )
    OR
    ? REGEXP t1.NotifyRuleset
)
AND t2.Inactive = 0
GROUP BY t2.ObjId",
{ Slice => {} },
"area:$incref->{BoxArea}",
"type:$incref->{CallType} group:$incref->{CallGroup}",
@{[ join(' ', map { "unit:$_" } @{ $UnitArray } ) ]}
