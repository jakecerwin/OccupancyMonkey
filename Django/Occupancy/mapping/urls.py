from django.urls import path
from mapping import views

urlpatterns = [
    path('', views.demo, name='mapping-demo'),
    path('lab/', views.lab, name='mapping-lab'),
    path('uc2/', views.uc, name='mapping-uc2'),
    path('get-status', views.get_status, name="get-status"),
    path('data', views.data, name="data"),
    path('receive', views.data, name="receive"),

]