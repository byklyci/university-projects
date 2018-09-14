<html>
<head>
</head>
<body>

    <?php
        $servername = "localhost";
        $username = "root";
        $password = "";
        $dbname = "cmpe321hw3";

        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);

        // Check connection
        if ($conn->connect_error) {

            die("Connection failed: " . $conn->connect_error);
        }else{
        ?>
            <form action="create_employee_result.php" method="post">
                <p>First Name: <input type="text" name="FName" value = "" /></p>
                <p>Last Name: <input type="text" name="LName" value = "" /></p>
                <p><input type="submit" value = "Create Record"/></p>
            </form>

        <?php
        }
        $conn->close();
    ?>

</body>
</html>
