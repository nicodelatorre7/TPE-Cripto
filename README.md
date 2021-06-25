 # TP Cripto - 1Q2021 

Trabajo Práctico de Criptografia y Seguridad - 1Q2021 
Implementación en C -Secreto compartido con Esteganografía.

## Compilación y Ejecución del programa:
- Descargar el programa:
- Parado en la carpeta raiz ejecutar el siguiente comando para compilar:
   ```bash
   make
   ```
- Cuando termine la compilación, usted tendrá un ejecutable en la carpeta raiz llamado `ss` 
- Para ejecutar el programa usted tiene dos opciones:
    - Encriptar
    - Desencriptar 
- Para ambos casos usted deberá tener una carpeta con al menos 4 imagenes en formato `.bmp`.
- En caso que se quiera **encriptar**  el mensaje de una foto, debe ejecutar el siguiente comando:
```
./ss d path/a/la/imagen [k] path/a/la/carpeta/de/imagenes 
```

- En caso que se quiera **desencriptar** el mensaje oculto en varias imagenes, debe ejecutar el siguiente comando:
```
./ss r path/a/la/imagen/desencriptada [k] path/a/la/carpeta/de/imagenes 
```

En ambos casos, los valores posibles de `k` son 4, 5 o 6


### NOTA: ### 
- Tanto para **encriptar**  como para **desencriptar**, es necesario enviar los parametros en el orden mostrado anteriormente


## Integrantes
- de la Torre, Nicolás - 58546
- Oliva, Juan Martín - 58664
- Vindis, Davor - 58663

