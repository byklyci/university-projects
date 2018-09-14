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

            // Fetch the record
            $sql = "SELECT ID, PName, StartDate FROM project WHERE ID = " . $_GET['id'];
            $result = $conn->query($sql);

            // If the record actually exists
            if ($result->num_rows > 0) {
                ?>
                <form action="edit_project_result.php" method="post">

                <?php

                // Get the data
                $row = $result->fetch_assoc();
                ?>
                    <p>ID: <input type="text" name="ID" value = "<?php echo $row["ID"] ?>" readonly /></p>
                    <p>Project name: <input type="text" name="PName" value = "<?php echo $row["PName"] ?>" /></p>
                    <p>Start Date: <input type="text" name="StartDate" value = "<?php echo $row["StartDate"] ?>" /></p>
                    <p><input type="submit" value = "Save Changes" /></p>
                </form>
                <?php
            } else {
                echo "Record does not exist";
            }
        }
        $conn->close();
    ?>

</body>
</html>
