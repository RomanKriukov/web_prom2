<%@ LANGUAGE="JScript" CODEPAGE=1251 %>
<% 
Response.Expires = -1 
Session.Timeout = 480
%>
<!--  #include file="connstr.inc" -->
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
<meta name="description" content="��������, ������� ��������">
<meta name="title" content="��������">
	
<title>Prometey</title>
<% 
var s=''+Request.ServerVariables('Request_Method');
if (s.indexOf('POST')<0) {
  style=''+Request.QueryString("style");
  if (style=='alt') {
    var oldstyle=Session.Contents("Style");
    if (oldstyle=='style1.css') {style='style2.css';} else {style='style1.css';};
    Session.Contents("Style")=style;
  }
};
WriteStyle(); 
%>
</head>
<body>
<% WriteHeader(); %>
	<div id="container">
<% WriteMenu('default'); %>
		<div id="inner">
<!--
				<div id="greeting">
					<h1>Address</h1>
					<div class="contacts">
					</div>
					<div class="contacts">
					</div>
				</div>
				<div class="about">
				</div>
-->
		</div>
	</div> <!--end container-->
<% WriteFooter(); 
    var sessionUser = Session.Contents("UserId");
    var sessionPWD = Session.Contents("Password")
%>
    <script>
         document.addEventListener('DOMContentLoaded', function () {
            // ��������� ��������� �� ���� id
            var protectedLink = document.getElementById('protected-link');

            // ������ �������� ��䳿 ���� �� ��������
            protectedLink.addEventListener('click', function (event) {
                event.preventDefault(); // ��������� �������� �� ���������

                // �������� ��� ��� ����������� (����� ����������� ��� ���)
                var credentials = {
                    username: '<%= sessionUser %>',
                    password: '<%= sessionPWD %>',
                };
                // ³���������� POST ����� ��� �����������
                fetch('http://localhost:5000/api/login', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify(credentials),
                    credentials: 'include' // �������� ���������� ����� � �������
                })
                    .then(response => {
                        console.log(response.status);
                        if (!response.ok) {
                            throw new Error('����� ��� ��� �����');
                        }
                        // ���� ����������� ������� ������, ���������� �� ���� �������
                        window.location.href = protectedLink.href;
                    })
                    .catch(error => {
                        console.error('������� �����������:', error);
                        // ������� ������� ����������� (���������, ����������� ����������� ��� �������)
                    });
            });
        });
    </script>
</body>
</html>