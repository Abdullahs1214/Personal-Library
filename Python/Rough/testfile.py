from reportlab.lib.pagesizes import letter
from reportlab.pdfgen import canvas
from reportlab.lib import colors

out_path = "group-P1.pdf"
c = canvas.Canvas(out_path, pagesize=letter)
w, h = letter

def draw_entity(x, y, width, height, title, attrs, pk=None):
  # draw rectangle and title bar
  c.setStrokeColor(colors.black)
  c.setFillColor(colors.whitesmoke)
  c.rect(x, y - height, width, height, fill=1)
  c.setFillColor(colors.lightgrey)
  c.rect(x, y - 20, width, 20, fill=1)
  c.setFillColor(colors.black)
  c.setFont("Helvetica-Bold", 10)
  c.drawString(x + 4, y - 15, title)
  c.setFont("Helvetica", 9)
  line_y = y - 30
  for a in attrs:
    if pk and a == pk:
      c.setFont("Helvetica-Bold", 9)
      c.drawString(x + 6, line_y, a)
      text_width = c.stringWidth(a, "Helvetica-Bold", 9)
      c.line(x + 6, line_y - 1, x + 6 + text_width, line_y - 1)
      c.setFont("Helvetica", 9)
    else:
      c.drawString(x + 6, line_y, a)
    line_y -= 12

# Title
c.setFont("Helvetica-Bold", 14)
c.drawCentredString(w/2, h - 40, "ER Diagram — Online Retail Store (Part I)")

# Entities: positions chosen to fit single page
margin = 50
draw_entity(margin, h - 90, 180, 80, "Customers", ["customer_id", "name", "email", "shipping_addr"], pk="customer_id")
draw_entity(margin + 220, h - 90, 180, 80, "Products", ["product_id", "name", "category", "price", "stock_count"], pk="product_id")
draw_entity(margin + 440, h - 90, 180, 80, "Devices", ["device_id", "ip_address", "device_type"], pk="device_id")
draw_entity(margin + 100, h - 210, 220, 90, "Sessions", ["session_id", "customer_id (FK)", "start_time", "end_time"], pk="session_id")
draw_entity(margin + 360, h - 210, 220, 90, "Session_Devices", ["session_device_id", "session_id (FK)", "device_id (FK)", "source"], pk="session_device_id")
draw_entity(margin, h - 210, 220, 90, "Activities", ["activity_id", "session_id (FK)", "activity_time", "activity_type", "product_id (FK)"], pk="activity_id")
draw_entity(margin, h - 360, 180, 90, "Credit_Cards", ["credit_card_id", "card_number", "expiry_date", "cardholder_name"], pk="credit_card_id")
draw_entity(margin + 200, h - 360, 180, 90, "Payments", ["payment_id", "order_id (FK)", "payment_time", "amount", "success", "credit_card_id (FK)"], pk="payment_id")
draw_entity(margin + 420, h - 360, 220, 90, "Orders", ["order_id", "order_number", "customer_id (FK)", "session_id (FK)", "order_date", "shipping_addr", "total_amount"], pk="order_id")
draw_entity(margin + 420, h - 480, 220, 80, "Order_Items", ["order_item_id", "order_id (FK)", "product_id (FK)", "quantity", "unit_price", "discount"], pk="order_item_id")

# small footnote
c.setFont("Helvetica", 8)
c.drawString(margin, 60, "PK attributes are bold/underlined. (FK) marks foreign keys. Adjust notation to match lectures.")
c.showPage()
c.save()
print("Written", out_path)
