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
            <form action="create_project_result.php" method="post">
                <p>Name: <input type="text" name="PName" value = "" /></p>
                <p>Start Date: <input type="text" name="StartDate" value = "" placeholder="e.g. 2018-05-27" /></p>
                <p><input type="submit" value = "Create Record"/></p>
            </form>

        <?php
        }
        $conn->close();
    ?>

</body>
</html>
