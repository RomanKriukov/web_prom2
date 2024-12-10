docker compose -f .\docker-compose.dev.yml up

##
docker build . -t flaskwin

docker run -v .:c:\app -p 8080:5000 -it flaskwin