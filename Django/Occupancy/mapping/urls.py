from django.urls import path
from . import views

urlpatterns = [
    path('', views.home, name='mapping-home'),
    path('test/', views.test, name='mapping-test')
]