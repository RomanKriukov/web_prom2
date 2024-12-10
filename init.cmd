powershell.exe Start-Service sshd
powershell.exe Start-Service docker
echo y | docker container prune
docker run --name web_prom2 -d -v .:c:\app -p 8080:8080 flaskwin