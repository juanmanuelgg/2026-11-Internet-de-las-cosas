-- Initialize PostgreSQL user and database
CREATE USER dbadmin WITH PASSWORD 'uniandesIOT1234';
ALTER USER dbadmin CREATEDB;
CREATE DATABASE iot_data OWNER dbadmin;
GRANT ALL PRIVILEGES ON DATABASE iot_data TO dbadmin;
