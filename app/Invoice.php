<?php

namespace App;

use App\Services\FakturoidClientService;
use Illuminate\Auth\Authenticatable;
use Illuminate\Database\Eloquent\Collection;
use Laravel\Lumen\Auth\Authorizable;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Contracts\Auth\Authenticatable as AuthenticatableContract;
use Illuminate\Contracts\Auth\Access\Authorizable as AuthorizableContract;
use rikudou\CzQrPayment\QrPayment;
use App\Role;

class Invoice extends Model implements AuthenticatableContract, AuthorizableContract
{
    use Authenticatable, Authorizable;

    /**
     * The attributes that are mass assignable.
     *
     * @var array
     */
    protected $fillable = [
        'fakturoid_id', 'number', 'client', 'status', 'issued_on', 'taxable_fulfillment_due', 'due_on', 'currency', 'language', 'total', 'qr_url'
    ];

    /**
     * The attributes excluded from the model's JSON form.
     *
     * @var array
     */
    protected $hidden = [
        'fakturoid_id'
    ];

    public $lines = [];
    public $due;
    private $totalAmount = 0;
    private $fcs;
    private $text = null;
    public $client;

    const OPEN_STATUSES = array('open', 'sent', 'overdue');
    const CLOSED_STATUSES = array('paid', 'cancelled');

    public function __construct()
    {
        $this->fcs = new FakturoidClientService();
    }

    public function client()
    {
        return $this->belongsTo(Client::class, 'client', 'id');
    }

    public function generateQr()
    {
        $payment = QrPayment::fromAccountAndBankCode(env('ACCOUNT_CZ_NUMBER'),env('ACCOUNT_CZ_BANK'));

        $payment->setVariableSymbol($this->number);
        $payment->setAmount($this->total);
        $payment->setCurrency($this->currency);

        if (file_exists("qrs/$this->qr_url.png")) {
            $qrFileName = $this->qr_url;
        } else {
            $qrFileName = uniqid($this->number, true);
        }
        $payment->getQrCode()->writeToFile("qrs/$qrFileName.png");

        $this->qr_url = $qrFileName;
        return $this->qr_url;
    }

    public function getPdf()
    {
        $invoiceFileName = $this->qr_url;
        $invoicePdf = $this->fcs->getInvoicePdf($this->fakturoid_id);
        $count = 1;

        while ($invoicePdf->getStatusCode() !== 200) {
            sleep(2);
            $invoicePdf = $this->fcs->getInvoicePdf($this->fakturoid_id);
            $count++;

            if ($count >= 5 && $invoicePdf->getStatusCode() !== 200) {
                return false;
            }
        }

        $invoiceFile = fopen("invoices/$invoiceFileName.pdf", 'w');
        fwrite($invoiceFile, $invoicePdf->getBody());
        fclose($invoiceFile);

        return true;
    }

    public function setDue($dueTime)
    {
        $difference = $dueTime - time();
        if (0 >= $difference){
            $this->due = 0;
        } else {
            $this->due = round($difference / (24 * 60 * 60));
        }
    }

    public function setLine($description, $quantity, $unit, $price)
    {
        $this->lines[] = [
            'name' => $description,
            'quantity' => $quantity,
            'unit_name' => $unit,
            'unit_price' => $price
        ];
    }

    public function setText(String $text): void
    {
        $this->text = $text;
    }

    public function setLanguage(string $lang): void
    {
        $this->language = $lang;
    }

    private function addUpToTotalAmount($addition)
    {
        $this->totalAmount = $this->totalAmount + $addition;
    }

    public function getTotalAmount(): float
    {
        return $this->totalAmount;
    }

    // TODO: vyřešit už existující invoice
    public function createFakturoidInvoice()
    {
        $invoiceData = [
            'subject_id' => $this->client->fakturoid_id,
            // TODO: vyřešit, proč se nepropisuje do faktur
            'taxable_fulfillment_due' => $this->taxable_fulfillment_due,
            'due' => $this->due,
            'note' => $this->text,
            'lines' => $this->lines,
            'currency' => $this->currency,
            'language' => $this->language
        ];
        if ('EUR' === $this->currency)
        {
            $invoiceData['bank_account_id'] = 8440;
        }
        $fakturoidInvoice = $this->fcs->createInvoice($invoiceData)->getBody();
        $this->setFakturoidData($fakturoidInvoice);
        $this->generateQr();
        $this->client()->associate($this->client);
        $this->save();
    }

    public function setFakturoidData($fakturoidInvoice)
    {
        $this->fakturoid_id = $fakturoidInvoice->id;
        $this->number = $fakturoidInvoice->number;
        $this->status = $fakturoidInvoice->status;
        $this->issued_on = $fakturoidInvoice->issued_on;
        $this->due_on = $fakturoidInvoice->due_on;
        $this->currency = $fakturoidInvoice->currency;
        $this->language = $fakturoidInvoice->language;
        $this->total = $fakturoidInvoice->total;
    }

    public function setFullUrls()
    {
        $this->qr_full_url = "https://api-prod.debata21.cz/qrs/$this->qr_url.png"; // TODO: nastavovat adresu dynamicky
        if ($this->getPdf()) {
            $this->pdf_url = $this->qr_url;
            $this->pdf_full_url = "https://api-prod.debata21.cz/invoices/$this->pdf_url.pdf";
        }
    }

    public function setRegistrationFeeLines(Collection $registrationQuantifiedRoles, Event $event, string $lang = 'cs')
    {
        foreach ($registrationQuantifiedRoles as $reg) {
            $role = Role::findOrFail($reg->role);
            $prices = $role->prices()->where('event', $event->id)->get();
            // TODO: solve properly
            foreach ($prices as $price) {
                $this->currency = $price->getCurrency();
                $priceDescription = $price->translation()->first();
                if ('Accommodation' == $priceDescription->en) {
                    if (false == $reg->accommodation) {
                        continue;
                    }
                    if ($event->isDiscountAvailable()) {
                        $this->setLine('sleva za včasnou platbu', $reg->quantity, 'osob', -150);
                        $this->setDue($event->getDiscountTime());
                    }
                }
                $unitPrice = $price->getAmount();

                $description = $role->translation()->first()->cs.' - '.$priceDescription->cs;
                $unit = 'osob';
                if ('en' === $lang)
                {
                    $description = $role->translation()->first()->en.' - '.$priceDescription->en;
                    $unit = 'people';
                }

                $this->setLine($description, $reg->quantity, $unit, $unitPrice);
                $this->addUpToTotalAmount($reg->quantity * $unitPrice);
            }
        }
    }

    public function setMembershipFeeLines(Collection $registrationGroup)
    {
        $membershipsCount = 0;
        foreach ($registrationGroup as $registration) {
            $person = $registration->person()->first();
            // TODO: to be deleted if person required in registration
            if (null !== $person) {
                $membership = $person->membership()->first();
                if (null === $membership) {
                    $membership = \App\Membership::create([
                        'person' => $person->id,
                        'beginning' => date('Y-m-d'),
                        'end' => \App\Membership::setEndDate(date('Y'), date('n'))
                    ]);
                    $membershipsCount++;
                } elseif ($membership->isExpired()) {
                    $membership->update([
                        'end' => \App\Membership::setEndDate(date('Y'), date('n'))
                    ]);
                    $membershipsCount++;
                }
            }
        }
        if ($membershipsCount > 0) {
            // TODO: solve translations and maybe add surnames, change unit a set price dynamically
            $this->setLine('členský příspěvek', $membershipsCount, 'osob', 50);
        }
        $this->addUpToTotalAmount($membershipsCount * 50);
    }

    public function setMissingAdjudicatorFeeLine(int $teamsCount, int $adjudicatorsCount)
    {
        if ($adjudicatorsCount < $teamsCount)
        {
            $missingAdjudicatorsCount = $teamsCount - $adjudicatorsCount;
            $this->setLine('missing judge fee', $missingAdjudicatorsCount, 'judges', 30);
            $this->addUpToTotalAmount($missingAdjudicatorsCount * 30);
        }
    }

    public function getPeopleListForEmail(Collection $registrationGroup, string $lang = 'cs')
    {
        $people = array();
        foreach ($registrationGroup as $registration) {
            $person = $registration->person()->first();
            // TODO: to be deleted if person required in registration
            if (null !== $person) {
                if ('en' === $lang)
                {
                    $roleName = $registration->role()->first()->translation()->first()->en;
                }
                else
                {
                    $roleName = $registration->role()->first()->translation()->first()->cs;
                }
                $team = $registration->team()->first();
                if (null !== $team) {
                    $people[$roleName][$team->name][] = $person->name . ' ' . $person->surname;
                } else {
                    $people[$roleName]['emptyTeamName'][] = $person->name . ' ' . $person->surname;
                }
            }
        }
        return $people;
    }

    /**
     * Confirms whether the greybox invoice is different from Fakturoid invoice
     *
     * @param $invoice
     * @return boolean
     */
    public function isDifferentFromFakturoidInvoice($invoice): bool
    {
        if ($this->status !== $invoice->status) return true;
        if ($this->due_on !== $invoice->due_on) return true;
        if ($this->total !== $invoice->total) return true;
        return false;
    }

    /**
     * Updates invoice data fields from Fakturoid invoice
     *
     * @param $invoice
     * @return void
     */
    public function updateFromFakturoid($invoice): void
    {
        $this->update([
            'number' => $invoice->number, // TODO: solve client
            'status' => $invoice->status,
            'issued_on' => $invoice->issued_on,
            'taxable_fulfillment_due' => $invoice->taxable_fulfillment_due,
            'due_on' => $invoice->due_on,
            'currency' => $invoice->currency,
            'language' => $invoice->language,
            'total' => $invoice->total
        ]);
    }
}
