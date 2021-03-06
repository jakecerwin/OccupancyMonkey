# Generated by Django 4.0.3 on 2022-04-05 14:19

from django.utils import timezone, dateformat
from django.db import migrations, models
from django.core.management import call_command

def forward_func(apps, schema_editor):
    print("Initializing Table database")
    table_ids = [0, 1, 2, 3]

    Table = apps.get_model('mapping', 'Table')

    for table in table_ids:
        obj = Table(id = table, status="unoccupied", update_time=timezone.now())
        obj.save()
        print(obj)



def reverse_func(apps, schema_editor):
    print('reverse')


class Migration(migrations.Migration):

    dependencies = [
        ('mapping', '0001_initial'),
    ]

    operations = [
        migrations.RunPython(forward_func, reverse_func)
    ]

