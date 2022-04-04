from django.urls import path
from mapping import views

urlpatterns = [
    path('', views.home, name='mapping-home'),
    path('test/', views.test, name='mapping-test'),
    path('get-status', views.get_status, name="get-status")
]