from django.shortcuts import render
from django.http import HttpResponse
# Create your views here.

def home(request):
  return render(request, 'map/map.html')

def test(request):
  return HttpResponse('<h1>MapTest</h1>')