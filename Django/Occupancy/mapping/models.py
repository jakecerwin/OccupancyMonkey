from django.db import models

# Create your models here.

class Table(models.Model):
  id = models.IntegerField(primary_key=True)
  table_id = models.IntegerField
  status = models.CharField(max_length=10)
  update_time = models.DateTimeField()