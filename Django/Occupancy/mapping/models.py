from django.db import models

# Create your models here.

class Table(models.Model):
  id = models.AutoField(primary_key=True)
  table_id = models.IntegerField
  status = models.CharField(max_length=10)
  update_time = models.DateTimeField()

  def __str__(self):
        return 'Table(id=' + str(self.id) + 'status=' + str(self.status) + ')'