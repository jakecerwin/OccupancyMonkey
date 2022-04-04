from django.shortcuts import render, redirect, get_object_or_404

from django.http import HttpResponse
from mapping.models import Table 
from django.utils import timezone, dateformat

import json

# Create your views here.

def home(request):
  return render(request, 'mapping/mapping.html')

def test(request):
  return HttpResponse('<h1>MapTest</h1>')

def get_status(request):
  response_data = [] 
  items = Table.objects.all()
  for table in items:
    item = {
      'table': table.id,
      'status': table.status
    }
    response_data.append(item)

  response_json = json.dumps(response_data)
  response = HttpResponse(response_json, content_type='application/json')
  response['Access-Control-Allow-Origin'] = '*'

  return response


def data(request):
  data = request.POST

  print(len(data))
  table_id = data['table_id']
  status = data['status']

  table = Table.objects.filter(table_id=table_id).all()[0]

  table.status = status
  table.save()


  