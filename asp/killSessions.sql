ALTER PROCEDURE killSessions
	@login VARCHAR(50),
	@program VARCHAR(50)
AS
BEGIN

	SET NOCOUNT ON
	SET ANSI_WARNINGS OFF
	
	DECLARE @sid INT,
			@sql VARCHAR(20)
		
	DECLARE sidCursor CURSOR LOCAL FOR
	SELECT session_id
	FROM sys.dm_exec_sessions
	WHERE login_name LIKE IIF(@login IS NULL, '%', @login)
		AND program_name LIKE '%'+@program+'%'
		--AND status NOT LIKE '%running%'
		AND database_id = DB_ID('fa')
		AND session_id<>@@SPID

    OPEN sidCursor
    FETCH NEXT FROM sidCursor
    INTO @sid

    WHILE @@FETCH_STATUS=0
    BEGIN		
		SET @sql = 'KILL ' + CAST(@sid AS VARCHAR(10))
        EXEC(@sql)
        FETCH NEXT FROM sidCursor
        INTO @sid
    END

END


