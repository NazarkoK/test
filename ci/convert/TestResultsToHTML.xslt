<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:output method="html" indent="yes"/>
	<xsl:template match="/">
		<html>
			<head>
				<title>Test Results</title>
				<style>
					body {
					font-family: Arial, sans-serif;
					background-color: #f8f9fa;
					color: #333;
					margin: 0;
					padding: 20px;
					}
					h1 {
					color: #0056b3;
					text-align: center;
					}
					table {
					width: 80%;
					margin: 20px auto;
					border-collapse: collapse;
					background: #fff;
					box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
					}
					th, td {
					padding: 12px 15px;
					text-align: left;
					border-bottom: 1px solid #ddd;
					}
					th {
					background-color: #007bff;
					color: white;
					}
					tr:nth-child(even) {
					background-color: #f2f2f2;
					}
					.passed {
					color: green;
					font-weight: bold;
					}
					.failed {
					color: red;
					font-weight: bold;
					}
					.error-message {
					color: #d9534f;
					font-size: 0.9em;
					}
				</style>
			</head>
			<body>
				<h1>Test Results</h1>
				<table>
					<tr>
						<th>Test Name</th>
						<th>Status</th>
						<th>Duration (s)</th>
						<th>Error Message</th>
					</tr>
					<xsl:for-each select="//testcase">
						<tr>
							<td>
								<xsl:value-of select="@name"/>
							</td>
							<td>
								<xsl:choose>
									<xsl:when test="failure">
										<span class="failed">Failed</span>
									</xsl:when>
									<xsl:otherwise>
										<span class="passed">Passed</span>
									</xsl:otherwise>
								</xsl:choose>
							</td>
							<td>
								<xsl:value-of select="@time"/>
							</td>
							<td>
								<xsl:if test="failure">
									<span class="error-message">
										<xsl:value-of select="failure/@message"/>
									</span>
								</xsl:if>
							</td>
						</tr>
					</xsl:for-each>
				</table>
			</body>
		</html>
	</xsl:template>
</xsl:stylesheet>