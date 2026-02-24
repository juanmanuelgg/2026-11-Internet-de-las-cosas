```bash
python3 -m venv ./.venv
source ./.venv/Scripts/activate
pip3 install -r requirements.txt
```

```bash
pip3 install Django
pip3 install Django-crontab
pip3 install psycopg2
pip3 install ldap3
pip3 install django_cron
pip3 install requests

pip3 freeze > requirements.txt

python3 manage.py makemigrations
python3 manage.py migrate
python3 manage.py generate_data 
```