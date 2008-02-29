set feedback OFF
set pagesize 0
set linesize 1000
prompt -- Drop functions
select 'drop ' || object_type || ' ' || object_name || ';' as command
from user_procedures where object_type='FUNCTION' order by object_name;
prompt
prompt -- Drop procedures
select 'drop ' || object_type || ' ' || object_name || ';' from user_procedures where object_type='PROCEDURE' order by object_name;
prompt
prompt -- Drop packages
select 'drop ' || object_type || ' ' || object_name || ';' from user_procedures where object_type='PACKAGE' order by object_name;
prompt
prompt -- Drop views
select 'drop view ' || view_name || ';' from user_views order by view_name;
prompt
prompt -- Drop sequences
select 'drop sequence ' || SEQUENCE_NAME || ';' from user_sequences order by sequence_name;
prompt
prompt -- Drop foreign key constraints
select 'alter table ' || table_name || ' drop constraint ' || constraint_name || ';' from user_constraints where constraint_type='R';
prompt
prompt -- Drop tables
select 'drop table ' || table_name || ';' from user_tables order by table_name;
exit;
