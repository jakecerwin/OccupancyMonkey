from django.shortcuts import render, redirect, get_object_or_404
from django.urls import reverse

from django.http import HttpResponse
from mapping.models import Table 
from django.utils import timezone, dateformat


from django.views.decorators.csrf import csrf_exempt

import json

# Create your views here.

def _my_json_error_response(message, status=200):
    # You can create your JSON by constructing the string representation yourself (or just use json.dumps)
    response_json = '{ "error": "' + message + '" }'
    return HttpResponse(response_json, content_type='application/json', status=status)



def home(request):
  return render(request, 'mapping/mapping.html')

def test(request):
  return HttpResponse('<h1>MapTest</h1>')

def get_status(request):
  response_data = [] 
  items = Table.objects.all()
  print(items)
  for table in items:
    item = {
      'table': table.id,
      'status': table.status
    }
    response_data.append(item)

  if len(response_data) == 0:
    response_data.append({'message' : 'empty'})

  response_json = json.dumps(response_data)
  response = HttpResponse(response_json, content_type='application/json')
  response['Access-Control-Allow-Origin'] = '*'

  return response

@csrf_exempt
def data(request):
  print("entering data")
  data = json.loads(request.read())]
  

  #key = data['key'] 
  table_id = data['table_id']
  status = data['status']
  
 
  table = get_object_or_404(Table, id=table_id)
  table.status = status
  table.save()

  return HttpResponse('data received OK')

def test_post_entry_view_good_post_data(self):
  '''post_entry view should return a 200 status if valid
  '''

  data = {'DHTP Data': ['10', '50.296', '50.94', '50.418', '50.425', '50.431', '50.94'],
      'Test String': 'My Test String'}

  request_url = reverse('data') 
  response = self.client.post(request_url, content_type='application/json', 
      data=json.dumps(data))

  # Should return a 200 response indicating ok
  self.assertEqual(response.status_code, 200) 