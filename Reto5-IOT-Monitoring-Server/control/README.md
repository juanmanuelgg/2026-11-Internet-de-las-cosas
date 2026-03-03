```bash
python3 -m venv ./.venv
source ./.venv/Scripts/activate
pip3 install -r requirements.txt

pip3 install django
pip3 install asgiref
pip3 install paho-mqtt
pip3 install psycopg2-binary
pip3 install python-dateutil
pip3 install crispy-bootstrap4==2022.1
pip3 install schedule

pip3 freeze > requirements.txt

python3 manage.py makemigrations
python3 manage.py migrate
python3 manage.py generate_data 
```